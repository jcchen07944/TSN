#include <math.h>
#include <stdio.h>
#include <cmath>

#include "Port.h"
#include "Constants.h"
#include "Utility.h"

SWPort::SWPort(int port_num, Switch *sw, double rate) {
    this->port_num = port_num;
    this->sw = sw;
    this->rate = rate;

    for(int i = 0; i < 8; i++) {
        t_queue.push_back(new std::queue<Packet*>);
        t_priority_queue.push_back(new std::priority_queue<Packet*, std::vector<Packet*>, Comparison>);
    }

    _pforward = nullptr;
    _tforward = 0;

    cycle = 1;
    std::priority_queue<Packet*, std::vector<Packet*>, TRComparison> new_queue;
    scheduled_queue.push_back(new_queue);
    gate_control_list.push_back(-1);
    current_slot = 0;
}

SWPort::~SWPort() {

}

void SWPort::receivePacket(Packet* packet) {
    if(packet->reservation_state == LISTENER_ACCEPT) {
        acceptTimeSlot(packet);
    }
    else if(packet->reservation_state == LISTENER_REJECT) {
        offset_table.erase(_pforward->flow_id);
    }
    sw->receivePacket(port_num, packet);
}

void SWPort::run(long long time) {
    current_slot = floor((time / (slot_duration*100) + cycle - 1) % cycle);
    if(_pforward != nullptr) {
        if(time >= _tforward) {
            // Forwarding finish
            if(device == SWITCH)
                sw_port->receivePacket(_pforward);
            else
                ed_port->receivePacket(_pforward);
            _pforward = nullptr;
        }
    }
    if(_pforward == nullptr) {
        if(time_reservation_enable) {
            if(gate_control_list[current_slot] != -1) {
                if(scheduled_queue[gate_control_list[current_slot]].size() != 0) {
                    _pforward = scheduled_queue[gate_control_list[current_slot]].top();
                    printf("Switch %d, Flow %d, Time %.4f, Slot %d\n", sw->ID, _pforward->p_flow_id, time/100.0, current_slot);
                    _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                    scheduled_queue[gate_control_list[current_slot]].pop();
                    return;
                }
            }
            if(be_queue.size() > 0) {
                //printf("%f %f\n", ((double)be_queue.front()->p_size / rate / us * 100.0d + time) / (slot_duration*100), time / (slot_duration*100));
                int be_slot_count = 0;
                for(int i = 1; i < cycle; i++) {
                    if(gate_control_list[(current_slot + i) % cycle] != -1)
                        break;
                    be_slot_count++;
                }
                if(floor(((double)be_queue.front()->p_size / rate / us * 100.0d + time) / (slot_duration*100)) > floor(time / (slot_duration*100)) + be_slot_count)
                    return;
                _pforward = be_queue.front();
                if(_pforward->reservation_state == TALKER_ATTRIBUTE)
                    if(!reserveTimeSlot(_pforward))
                        _pforward = nullptr;
                _tforward = time + (double)be_queue.front()->p_size / rate / us * 100.0d;
                be_queue.pop();
            }
        }
        else if(ats_enable) {

        }
        // Strict Priority
        // FIFO
        else if(!priority_queue_enable) {
            for(int i = t_queue.size() - 1; i >= 0; i--) {
                if(t_queue[i]->size() != 0) {
                    // Dequeue packet and put to forwarding state
                    _pforward = t_queue[i]->front();
                    _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                    t_queue[i]->pop();
                    break;
                }
            }
        }
        // Priority Queue
        else {
            for(int i = t_priority_queue.size() - 1; i >= 0; i--) {
                if(t_priority_queue[i]->size() != 0) {
                    _pforward = t_priority_queue[i]->top();
                    _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                    t_priority_queue[i]->pop();
                    break;
                }
            }
        }
    }
}

bool SWPort::reserveTimeSlot(Packet *packet) {
    int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int

    // Extend cycle
    if(cycle % slots_per_period != 0) {
        Utility utility;
        int new_cycle = utility.lcm(cycle, slots_per_period);
        for(int i = cycle; i < new_cycle; i++) {
            if(time_slot.find(i - cycle) != time_slot.end()) {
                time_slot[new_cycle] = time_slot[i - cycle];
                gate_control_list.push_back(gate_control_list[i - cycle]);
            }
            else
                gate_control_list.push_back(-1);
        }
        cycle = new_cycle;
    }

    // Find time-slots
    slots_per_period = packet->period / slot_duration;
    bool can_reserve;
    for(int i = 0; i < slots_per_period; i++) { // Maximum slot offset
        can_reserve = true;
        int old_slot_need = -1;
        for(int j = 0; j < cycle / slots_per_period; j++) { // How many cycle need to check

            int next_time_slot = j * slots_per_period + (i + (int)ceil((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;
            printf("%d %d %d\n", sw->ID, next_time_slot, time_slot[next_time_slot]);
            if(time_slot[next_time_slot] == (int)std::round((double)slot_duration * us * link_speed)) { // Reserve first time-slot
                can_reserve = false;
                break;
            }

            int slot_need = (int)ceil((packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) / std::round((double)slot_duration * us * link_speed));
            slot_need = slot_need < 0? 1 : slot_need+1;
            //printf("%d\n", slot_need);
            if(old_slot_need != -1 && old_slot_need != slot_need) { // Make sure the delay in every cycle are same.
                can_reserve = false;
                break;
            }
            old_slot_need = slot_need;

            for(int k = 1; k < slot_need; k++) {
                if(i + k > slots_per_period) { // If other time-slot exceed the period
                    can_reserve = false;
                    break;
                }
                else if(k != slot_need - 1) {
                    if(time_slot.find(next_time_slot + k) != time_slot.end()) {
                        if(time_slot[next_time_slot + k] != 0) {
                            can_reserve = false;
                            break;
                        }
                    }
                }
                else {
                    if(time_slot.find(next_time_slot + k) != time_slot.end()) {
                        if(time_slot[next_time_slot + k] > packet->packet_size - k * (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) {
                            can_reserve = false;
                            break;
                        }
                    }
                }
            }
        }
        if(can_reserve) {
            int next_time_slot = (i + (int)ceil((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;
            int slot_need = (int)ceil((packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) / std::round((double)slot_duration * us * link_speed));
            slot_need = slot_need < 0? 1 : slot_need + 1;
            //printf("%d %d\n", sw->ID, slot_need);
            offset_table[packet->flow_id] = i;
            queue_table[packet->flow_id] = findAcceptQueueID(packet, i);
            last_transmission_time = packet->start_transmission_time;

            /**********BUG*********/
            /** Here need to fix **/
            /**********************/
            if(packet->acc_slot_count == 0) // Send by EndDevice
                packet->start_transmission_time += (i + slot_need)*slot_duration;
            else
                packet->start_transmission_time += (i + slot_need)*slot_duration;
            packet->acc_slot_count += (i + slot_need);
            printf("%d %.4f\n", sw->ID, packet->start_transmission_time);
            //printf("%d %d\n", sw->ID, packet->acc_slot_count);
            break;
        }
    }
    if(!can_reserve) {
        Packet *new_packet = new Packet(packet);
        new_packet->source = packet->destination;
        new_packet->destination = packet->source;
        new_packet->acc_slot_count = packet->acc_slot_count;
        new_packet->reservation_state = LISTENER_REJECT;
        new_packet->flow_id = packet->flow_id;
        sw->receivePacket(port_num, new_packet);
        delete packet;
        return false;
    }
    return true;
}

void SWPort::acceptTimeSlot(Packet *packet) {
    packet->start_transmission_time = last_transmission_time;
    //printf("%d %.4f\n", sw->ID, packet->start_transmission_time);

    queue_table[packet->flow_id] = findAcceptQueueID(packet, offset_table[packet->flow_id]);
    printf("%d %d\n", sw->ID, queue_table[packet->flow_id]);

    int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int
    for(int i = 0; i < cycle / slots_per_period; i++) { // How many time-slot need to check
        int next_time_slot = i * slots_per_period + (offset_table[packet->flow_id] + (int)ceil((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;

        //printf("%d %d\n", sw->ID, next_time_slot);
        if(packet->packet_size < (int)std::round((double)slot_duration * us * link_speed) - time_slot[next_time_slot]) { // If only reserve one time-slot
            time_slot[next_time_slot] += packet->packet_size;
            continue;
        }

        int slot_need = (int)ceil((packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) / std::round((double)slot_duration * us * link_speed));
        slot_need = slot_need < 0? 1 : slot_need+1;

        for(int k = 0; k < slot_need; k++) {
            gate_control_list[next_time_slot + k] = queue_table[packet->flow_id];
            if(k != slot_need - 1) {
                time_slot[next_time_slot + k] = (int)std::round((double)slot_duration * us * link_speed);
            }
            else {
                if(time_slot.find(next_time_slot + k) == time_slot.end())
                    time_slot[next_time_slot + k] = 0;
                time_slot[next_time_slot + k] += (packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) * slot_need + time_slot[next_time_slot]);
            }
        }
    }

    Packet *new_packet = new Packet(packet);
    new_packet->period = packet->period;
    new_packet->packet_size = packet->packet_size;
    new_packet->flow_id = packet->flow_id;
    new_packet->start_transmission_time = packet->start_transmission_time;
    reserved_table[packet->flow_id] = new_packet;
}

int SWPort::findAcceptQueueID(Packet *packet, int offset) {
    bool *queue_occupy_table = new bool[scheduled_queue.size()];
    std::fill(queue_occupy_table, queue_occupy_table + scheduled_queue.size(), false);

    int slots_per_period = packet->period / slot_duration;
    for(int j = 0; j < cycle / slots_per_period; j++) { // How many cycle need to check
        int start_time_slot = j * slots_per_period + (offset + (int)floor((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;
        bool old_queue_check = true;
        for(int i = offset - 1; i >= 0; i--) {
            int next_time_slot = j * slots_per_period + (i + (int)floor((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;
            if(gate_control_list[start_time_slot] != -1) { // Use common queue with the same gate time
                if(gate_control_list[next_time_slot] != gate_control_list[start_time_slot])
                    old_queue_check = false;
                if(!old_queue_check)
                    if(gate_control_list[next_time_slot] != -1)
                        queue_occupy_table[gate_control_list[next_time_slot]] = true;
            }
            else {
                if(gate_control_list[next_time_slot] != -1)
                    queue_occupy_table[gate_control_list[next_time_slot]] = true;
            }
        }
    }

    int queue_id = -1;
    for(size_t i = 0; i < scheduled_queue.size(); i++) {
        if(queue_occupy_table[i] == false) {
            queue_id = i;
            break;
        }
    }
    if(queue_id == -1) {
        queue_id = scheduled_queue.size();
        std::priority_queue<Packet*, std::vector<Packet*>, TRComparison> new_queue;
        scheduled_queue.push_back(new_queue);
    }

    delete queue_occupy_table;
    return queue_id;
}
