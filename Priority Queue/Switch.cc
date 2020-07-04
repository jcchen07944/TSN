#include <vector>
#include <stdio.h>
#include <math.h>

#include "Switch.h"
#include "Constants.h"
#include "Utility.h"

Switch::Switch(int ID) {
    _time = 0;
    _accumulate_sequence_number = 0;

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
        int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int

        // Extend cycle
        if(cycle % slots_per_period != 0) {
            Utility utility;
            int new_cycle = utility.lcm(cycle, slots_per_period);
        }

        bool can_reserve = true;
        for(int i = 0; i < slots_per_period; i++) {
            for(int j = 0; j < cycle / slots_per_period; j++) {
                int next_time_slot = j * slots_per_period + i + floor(packet->start_transmission_time / slot_duration);
                if(time_slot.find(next_time_slot) != time_slot.end()) {
                    if(time_slot[next_time_slot] + packet->p_size > (int)((double)slot_duration * us * link_speed)) {
                        can_reserve = false;
                        break;
                    }
                }
            }
            if(can_reserve) {
                packet->acc_slot_count += (i + 1);
                packet->start_transmission_time = (int)(packet->start_transmission_time + (i + 1) * slot_duration) % (int)packet->period;
                offset_table[packet->p_flow_id] = i;
                break;
            }
        }
        Packet *new_packet = new Packet(packet);
        new_packet->source = packet->destination;
        new_packet->destination = packet->source;
        new_packet->acc_slot_count = packet->acc_slot_count;
        new_packet->reservation_state = LISTENER_REJECT;
        port[routing_table[new_packet->destination]]->t_queue[new_packet->p_priority]->push(new_packet);
        delete packet;
        return;
    }
    // Listener Accept : Reserve
    else if(packet->reservation_state == LISTENER_ACCEPT) {
        int slots_per_period = packet->period / slot_duration; // Now assume that slots per period are int
        for(int i = 0; i < cycle / slots_per_period; i++) {
            int next_time_slot = i * slots_per_period + offset_table[packet->p_flow_id] + floor(packet->start_transmission_time / slot_duration);
            if(time_slot.find(next_time_slot) == time_slot.end())
                time_slot[next_time_slot] = 0;
            time_slot[next_time_slot] += packet->p_size;
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
