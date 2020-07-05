#include <vector>
#include <stdio.h>
#include <math.h>
#include <cmath>

#include "Switch.h"
#include "Constants.h"
#include "Utility.h"

Switch::Switch(int ID) {
    _time = 0;
    _accumulate_sequence_number = 0;
    cycle = 1;

    this->ID = ID;
    rate = link_speed;
}

Switch::~Switch() {
    for(size_t i = 0; i < port.size(); i++) {
        delete port[i];
    }
}

void Switch::connectNextHop(int port_num, SWPort *sw_port) {
    port[port_num]->addDevice(sw_port);
}

void Switch::connectNextHop(int port_num, EDPort *ed_port) {
    port[port_num]->addDevice(ed_port);
}

SWPort* Switch::newPort() {
    int port_num = port.size();
    SWPort *sw_port = new SWPort(port_num, this, rate);
    port.push_back(sw_port);
    return sw_port;
}

void Switch::receivePacket(int port_num, Packet* packet) {
    if(packet->broadcast) {
        if(routing_table.find(packet->source) == routing_table.end()) {
            routing_table[packet->source] = port_num;
        }
        else
            return;
    }

    // Talker-Attribute : Find a time-slot
    if(packet->reservation_state == TALKER_ATTRIBUTE) {
        int slots_per_period = packet->period / slot_duration;

        // Extend cycle
        if(cycle % slots_per_period != 0) {
            Utility utility;
            int new_cycle = utility.lcm(cycle, slots_per_period);
            for(auto& reserved_flow : reserved_table) {
                slots_per_period = reserved_flow.second->period / slot_duration;
                for(int j = cycle / slots_per_period; j < new_cycle / slots_per_period; j++) {
                    int next_time_slot = j * slots_per_period + offset_table[reserved_flow.second->p_flow_id] + std::round(reserved_flow.second->start_transmission_time / slot_duration);
                    if(time_slot.find(next_time_slot) == time_slot.end())
                        time_slot[next_time_slot] = 0;
                    time_slot[next_time_slot] += reserved_flow.second->packet_size;
                }
            }
            cycle = new_cycle;
        }

        slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int
        bool can_reserve;
        for(int i = 0; i < slots_per_period; i++) { // Maximum slot offset
            can_reserve = true;
            for(int j = 0; j < cycle / slots_per_period; j++) {
                int next_time_slot = j * slots_per_period + (i + (int)std::round(packet->start_transmission_time / slot_duration)) % slots_per_period;
                if(time_slot.find(next_time_slot) == time_slot.end())
                    time_slot[next_time_slot] = 0;
                //printf("%d %d\n", ID, next_time_slot);
                if(time_slot[next_time_slot] + packet->packet_size > std::round((double)slot_duration * us * link_speed)) {
                    can_reserve = false;
                    break;
                }
            }
            if(can_reserve) {
                packet->acc_slot_count += (i + 1);
                packet->start_transmission_time = (int)std::round(packet->start_transmission_time + (i + 1) * slot_duration) % (int)std::round(packet->period);
                //printf("%d %.4f\n", ID, packet->start_transmission_time);
                offset_table[packet->p_flow_id] = i;
                break;
            }
        }
        if(!can_reserve) {
            Packet *new_packet = new Packet(packet);
            new_packet->source = packet->destination;
            new_packet->destination = packet->source;
            new_packet->acc_slot_count = packet->acc_slot_count;
            new_packet->reservation_state = LISTENER_REJECT;
            port[routing_table[new_packet->destination]]->t_queue[new_packet->p_priority]->push(new_packet);
            delete packet;
            return;
        }
    }
    // Listener Accept : Reserve
    else if(packet->reservation_state == LISTENER_ACCEPT) {
        packet->start_transmission_time = (int)std::round(packet->start_transmission_time - (offset_table[packet->p_flow_id] + 1) * slot_duration + (int)std::round(packet->period)) % (int)std::round(packet->period);
        int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int
        for(int i = 0; i < cycle / slots_per_period; i++) {
            int next_time_slot = i * slots_per_period + offset_table[packet->p_flow_id] + floor(packet->start_transmission_time / slot_duration);
            if(time_slot.find(next_time_slot) == time_slot.end())
                time_slot[next_time_slot] = 0;
            time_slot[next_time_slot] += packet->packet_size;
            printf("%d %d\n", ID, next_time_slot);
        }
        Packet *new_packet = new Packet(packet);
        reserved_table[packet->p_flow_id] = new_packet;
    }
    // Listener Reject : Resume
    else if(packet->reservation_state == LISTENER_REJECT) {
        offset_table.erase(packet->p_flow_id);
    }

    if(!priority_queue_enable) {
        if(packet->broadcast) {
            for(size_t i = 0; i < port.size(); i++) {
                if((int)i != routing_table[packet->source]) {
                    Packet *newPacket = new Packet(packet);
                    port[i]->t_queue[packet->p_priority]->push(newPacket);
                }
            }
            delete packet;
        }
        else
            port[routing_table[packet->destination]]->t_queue[packet->p_priority]->push(packet);
    }
    else {
        if(packet->broadcast) {
            for(size_t i = 0; i < port.size(); i++) {
                if((int)i != routing_table[packet->source]) {
                    Packet *newPacket = new Packet(packet);
                    newPacket->sequence_number = _accumulate_sequence_number++;
                    port[i]->t_priority_queue[packet->p_priority]->push(newPacket);
                }
            }
            delete packet;
        }
        else {
            packet->sequence_number = _accumulate_sequence_number++;
            port[routing_table[packet->destination]]->t_priority_queue[packet->p_priority]->push(packet);
        }
    }
}

void Switch::run() {
    for(SWPort *p : port)
        p->run(_time);

    _time++;
}
