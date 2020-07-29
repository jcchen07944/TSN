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

    cycle = 3;
    current_slot = 0;

    for(int i = 0; i < 8; i++)
        group_eligibility_time.push_back(0);
}

SWPort::~SWPort() {

}

void SWPort::receivePacket(Packet* packet) {
    if(packet->reservation_state == LISTENER_ACCEPT) {
        if(RESERVATION_MODE == TIME_RESERVATION)
            acceptTimeSlot(packet);
        else if(RESERVATION_MODE == ATS)
            acceptBandwidth(packet);
    }
    else if(packet->reservation_state == LISTENER_REJECT) {
        if(RESERVATION_MODE == TIME_RESERVATION)
            offset_table.erase(offset_table.find(packet->flow_id));
        else if(RESERVATION_MODE == ATS)
            ats_scheduler_table.erase(ats_scheduler_table.find(packet->flow_id));
    }
    sw->receivePacket(port_num, packet);
}

void SWPort::sendPacket(Packet* packet) {
    if(RESERVATION_MODE == TIME_RESERVATION) {
        if(offset_table.find(packet->p_flow_id) != offset_table.end()) {
            std::pair<Packet*, long long> scheduled_packet;
            scheduled_packet.first = packet;
            scheduled_packet.second = (long long)getEligibilitySlot(packet);
            scheduled_buffer.push_back(scheduled_packet);
            //printf("Switch %d, Flow %d, %d\n", sw->ID, packet->p_flow_id, getEligibilitySlot(packet));
        }
        else {
            be_queue.push(packet);
        }
    }
    else if(RESERVATION_MODE == ATS) {
        if(packet->p_priority == 0) {
            be_queue.push(packet);
        }
        else {
            std::pair<Packet*, long long> scheduled_packet;
            scheduled_packet.first = packet;
            scheduled_packet.second = ats_scheduler_list[ats_scheduler_table[packet->p_flow_id]]->getEligibilityTime(packet, group_eligibility_time);
            //printf("Switch %d, Flow %d, %lld\n", sw->ID, packet->p_flow_id, scheduled_packet.second);
            scheduled_buffer.push_back(scheduled_packet);
        }
    }
}

int SWPort::getEligibilitySlot(Packet* packet) {
    return (current_slot + offset_table[packet->p_flow_id] + 1) % cycle;
}

void SWPort::run(long long time) {
    current_slot = (int)floor(time / (slot_duration*100)) % cycle;
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
        if(RESERVATION_MODE == TIME_RESERVATION) {
            int selected_index = -1;
            for(size_t i = 0; i < scheduled_buffer.size(); i++) {
                if((int)scheduled_buffer[i].second == current_slot) {
                    if(_pforward == nullptr) {
                        _pforward = scheduled_buffer[i].first;
                        selected_index = i;
                    }
                    if(_pforward->p_size > scheduled_buffer[i].first->p_size) {
                        _pforward = scheduled_buffer[i].first;
                        selected_index = i;
                    }
                }
            }
            if(selected_index != -1) {
                //printf("Switch %d, Flow %d, Time %.4f, Slot %d\n", sw->ID, _pforward->p_flow_id, time/100.0, current_slot);
                _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                scheduled_buffer.erase(scheduled_buffer.begin() + selected_index);
            }
            else if(be_queue.size() > 0) {
                //printf("%f %f\n", ((double)be_queue.front()->p_size / rate / us * 100.0d + time) / (slot_duration*100), time / (slot_duration*100));
                int idle_size = 0; // bit
                idle_size += (int)floor((floor(time / (slot_duration*100) + 1) - time / (slot_duration*100)) * slot_duration * us * link_speed);
                for(int i = 1; i < cycle; i++) {
                    if(time_slot.find((current_slot + i) % cycle) == time_slot.end())
                        idle_size += (int)std::round((double)slot_duration * us * link_speed);
                    else if(time_slot[(current_slot + i) % cycle] == 0)
                        idle_size += (int)std::round((double)slot_duration * us * link_speed);
                    else
                        break;
                }
                if(be_queue.front()->p_size > idle_size)
                    return;
                //printf("%.4f\n", (double)be_queue.front()->p_size / rate / us * 100.0d);
                _pforward = be_queue.front();
                if(_pforward->reservation_state == TALKER_ATTRIBUTE)
                    if(!reserveTimeSlot(_pforward))
                        _pforward = nullptr;
                _tforward = time + (double)be_queue.front()->p_size / rate / us * 100.0d;
                be_queue.pop();
            }
        }
        else if(RESERVATION_MODE == ATS) {
            int selected_index = -1;
            int selected_prioirty = 0;
            for(size_t i = 0; i < scheduled_buffer.size(); i++) {
                if(scheduled_buffer[i].second <= time) {
                    if(selected_prioirty < scheduled_buffer[i].first->priority) {
                        _pforward = scheduled_buffer[i].first;
                        selected_index = i;
                        selected_prioirty = _pforward->priority;
                    }
                    else if(selected_prioirty == scheduled_buffer[i].first->priority) {
                        if(scheduled_buffer[selected_index].second > scheduled_buffer[i].second) {
                            _pforward = scheduled_buffer[i].first;
                            selected_index = i;
                        }
                    }
                }
            }
            if(selected_index != -1) {
                //printf("Switch %d, Flow %d, Time %.4f, Slot %d\n", sw->ID, _pforward->p_flow_id, time/100.0, current_slot);
                _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                scheduled_buffer.erase(scheduled_buffer.begin() + selected_index);
            }
            else if(be_queue.size() > 0) {
                _pforward = be_queue.front();
                if(_pforward->reservation_state == TALKER_ATTRIBUTE)
                    if(!reserveBandwidth(_pforward))
                        _pforward = nullptr;
                _tforward = time + (double)be_queue.front()->p_size / rate / us * 100.0d;
                be_queue.pop();
            }
        }
        // Strict Priority
        // FIFO
        else {
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
    }
}

bool SWPort::reserveTimeSlot(Packet *packet) {
    int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int

    // Extend cycle
    if(cycle % slots_per_period != 0) {
        Utility utility;
        int new_cycle = utility.lcm(cycle, slots_per_period);
        for(int i = cycle; i < new_cycle; i++) {
            if(time_slot.find(i - cycle) != time_slot.end())
                time_slot[new_cycle] = time_slot[i - cycle];
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
            //printf("%d %d %d\n", sw->ID, next_time_slot, time_slot[next_time_slot]);
            if(time_slot.find(next_time_slot) == time_slot.end())
                time_slot[next_time_slot] = 0;
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
                        if((int)std::round((double)slot_duration * us * link_speed) - time_slot[next_time_slot + k] < packet->packet_size - k * (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) {
                            can_reserve = false;
                            break;
                        }
                    }
                }
            }
        }
        if(can_reserve) {
            int next_time_slot = (i + (int)ceil((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;
            if(time_slot.find(next_time_slot) == time_slot.end())
                time_slot[next_time_slot] = 0;
            int slot_need = (int)ceil((packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) / std::round((double)slot_duration * us * link_speed));
            slot_need = slot_need < 0? 1 : slot_need + 1;
            //printf("%d %d\n", sw->ID, slot_need);
            offset_table[packet->flow_id] = i;
            last_transmission_time = packet->start_transmission_time;

            /**********BUG*********/
            /** Here need to fix **/
            /**********************/
            if(packet->acc_slot_count == 0) // Send by EndDevice
                packet->start_transmission_time += (i + slot_need)*slot_duration;
            else
                packet->start_transmission_time += (i + slot_need)*slot_duration;
            packet->acc_slot_count += (i + slot_need);
            //printf("%d %.4f\n", sw->ID, packet->start_transmission_time);
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

    int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int
    for(int i = 0; i < cycle / slots_per_period; i++) { // How many time-slot need to check
        int next_time_slot = i * slots_per_period + (offset_table[packet->flow_id] + (int)ceil((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration) - 1) % slots_per_period;

        //printf("%d %d\n", sw->ID, next_time_slot);
        if(time_slot.find(next_time_slot) == time_slot.end())
            time_slot[next_time_slot] = 0;

        int slot_need = (int)ceil((packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) + time_slot[next_time_slot]) / std::round((double)slot_duration * us * link_speed));
        slot_need = slot_need < 0? 1 : slot_need+1;

        for(int k = slot_need - 1; k >= 0; k--) {
            if(time_slot.find(next_time_slot + k) == time_slot.end())
                time_slot[next_time_slot + k] = 0;

            if(k == 0 && slot_need == 1) { // Only need one time-slot
                time_slot[next_time_slot] += packet->packet_size;
            }
            else if(k != slot_need - 1) {
                time_slot[next_time_slot + k] = (int)std::round((double)slot_duration * us * link_speed);
            }
            else {
                time_slot[next_time_slot + k] += (packet->packet_size - (int)std::round((double)slot_duration * us * link_speed) * (slot_need-1) + time_slot[next_time_slot]);
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

bool SWPort::reserveBandwidth(Packet *packet) {
    ATSScheduler *ats_scheduler = new ATSScheduler(ats_scheduler_list.size(), packet->priority);
    ats_scheduler->committed_burst_size = packet->packet_size;
    ats_scheduler->committed_information_rate = packet->packet_size / (packet->period * us);
    ats_scheduler_table[packet->flow_id] = ats_scheduler_list.size();
    ats_scheduler_list.push_back(ats_scheduler);

    Packet *new_packet = new Packet(packet);
    new_packet->period = packet->period;
    new_packet->packet_size = packet->packet_size;
    new_packet->flow_id = packet->flow_id;
    reserved_table[packet->flow_id] = new_packet;

    double max_delay = 0.0f;
    for(auto iter = reserved_table.begin(); iter != reserved_table.end(); iter++) {
        Packet *h = iter->second;
        if(h->priority != packet->priority)
            continue;

        int burst_size = 0;
        double higher_rate = 0.0f;
        for(auto jter = reserved_table.begin(); jter != reserved_table.end(); jter++) {
            Packet *g = jter->second;
            if(g->priority > h->priority) {
                burst_size += g->packet_size;
                higher_rate += (double)g->packet_size / (g->period * us);
            }
            if(g->priority == h->priority) {
                burst_size += g->packet_size;
            }
        }
        double tmp = ((double)(burst_size - h->packet_size + 12000) / (link_speed - higher_rate) + h->packet_size / link_speed) / us;
        max_delay = std::max(tmp, max_delay);
        //printf("%d %.4f\n", sw->ID, (burst_size - h->packet_size + 12000) / (link_speed - higher_rate));
    }

    reserved_table.erase(reserved_table.find(packet->flow_id));

    printf("%d %.4f\n", sw->ID, max_delay);
    if(max_delay > packet->per_hop_deadline) {
        Packet *new_packet = new Packet(packet);
        new_packet->source = packet->destination;
        new_packet->destination = packet->source;
        new_packet->reservation_state = LISTENER_REJECT;
        new_packet->flow_id = packet->flow_id;
        sw->receivePacket(port_num, new_packet);
        delete packet;
        return false;
    }
    return true;
}

void SWPort::acceptBandwidth(Packet *packet) {
    ats_scheduler_list[ats_scheduler_table[packet->flow_id]]->bucket_empty_time = 0;
    Packet *new_packet = new Packet(packet);
    new_packet->period = packet->period;
    new_packet->packet_size = packet->packet_size;
    new_packet->flow_id = packet->flow_id;
    reserved_table[packet->flow_id] = new_packet;
    return;
}
