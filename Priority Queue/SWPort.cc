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
    std::queue<Packet*> slot;
    offset_slot.push_back(slot);
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
    current_slot = floor(time / (slot_duration*100) % offset_slot.size());
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
            if(offset_slot[current_slot].size() > 0) {
                _pforward = offset_slot[current_slot].front();
                _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                offset_slot[current_slot].pop();
            }
            else if(be_queue.size() > 0) {
                //printf("%f %f\n", ((double)be_queue.front()->p_size / rate / us * 100.0d + time) / (slot_duration*100), time / (slot_duration*100));
                if(floor(((double)be_queue.front()->p_size / rate / us * 100.0d + time) / (slot_duration*100)) > floor(time / (slot_duration*100)))
                    return;
                _pforward = be_queue.front();
                if(_pforward->reservation_state == TALKER_ATTRIBUTE)
                    if(!reserveTimeSlot(_pforward))
                        _pforward == nullptr;
                _tforward = time + (double)be_queue.front()->p_size / rate / us * 100.0d;
                be_queue.pop();
            }
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
        for(auto& reserved_flow : reserved_table) {
            slots_per_period = reserved_flow.second->period / slot_duration;
            for(int j = cycle / slots_per_period; j < new_cycle / slots_per_period; j++) {
                int next_time_slot = j * slots_per_period + offset_table[reserved_flow.second->flow_id] + std::round(reserved_flow.second->start_transmission_time / slot_duration);
                if(time_slot.find(next_time_slot) == time_slot.end())
                    time_slot[next_time_slot] = 0;
                time_slot[next_time_slot] += reserved_flow.second->packet_size;
            }
        }
        cycle = new_cycle;
    }

    // Find a time-slot
    slots_per_period = packet->period / slot_duration;
    bool can_reserve;
    for(int i = 0; i < slots_per_period; i++) { // Maximum slot offset
        can_reserve = true;
        for(int j = 0; j < cycle / slots_per_period; j++) { // How many time-slot need to check
            int next_time_slot = j * slots_per_period + (i + (int)floor((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration)) % slots_per_period;
            if(time_slot.find(next_time_slot) == time_slot.end())
                time_slot[next_time_slot] = 0;
            //printf("%d\n", packet->start_transmission_time);
            //printf("%d %d %d\n", sw->ID, next_time_slot, time_slot[next_time_slot]);
            if(time_slot[next_time_slot] + packet->packet_size > std::round((double)slot_duration * us * link_speed)) {
                can_reserve = false;
                break;
            }
        }
        if(can_reserve) {
            packet->acc_slot_count += (i + 1);
            packet->start_transmission_time = packet->start_transmission_time + (i + 1)*slot_duration % (int)floor(packet->period);
            //printf("%d %.4f\n", sw->ID, packet->start_transmission_time);
            offset_table[packet->flow_id] = i;
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
    packet->start_transmission_time = (int)floor(packet->start_transmission_time - (offset_table[packet->flow_id] + 1) * slot_duration + (int)floor(packet->period)) % (int)floor(packet->period);
    int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int
    for(int i = 0; i < cycle / slots_per_period; i++) { // How many time-slot need to check
        int next_time_slot = i * slots_per_period + (offset_table[packet->flow_id] + (int)floor((packet->start_transmission_time + packet->packet_size/link_speed/us) / slot_duration)) % slots_per_period;
        if(time_slot.find(next_time_slot) == time_slot.end())
            time_slot[next_time_slot] = 0;
        time_slot[next_time_slot] += packet->packet_size;
        //printf("%d %d\n", sw->ID, next_time_slot);
    }
    // Extend offset_slot (One slot for send and other slots for receive.)
    for(int i = offset_slot.size(); i < offset_table[packet->flow_id] + 2; i++) {
        std::queue<Packet*> slot;
        //printf("%d\n", offset_table[packet->flow_id]);
        offset_slot.push_back(slot);
    }

    Packet *new_packet = new Packet(packet);
    new_packet->period = packet->period;
    new_packet->packet_size = packet->packet_size;
    new_packet->flow_id = packet->flow_id;
    new_packet->start_transmission_time = packet->start_transmission_time;
    reserved_table[packet->flow_id] = new_packet;
}
