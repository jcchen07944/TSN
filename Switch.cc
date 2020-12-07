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

    this->ID = ID;
    rate = link_speed;
    max_buffer_used = 0;
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
    // Spanning tree
    if(packet->broadcast) {
        if(routing_table.find(packet->source) == routing_table.end()) {
            routing_table[packet->source] = port_num;
        }
        else
            return;
    }

    /***********************/
    /** Switch Processing **/
    /***********************/
    if(RESERVATION_MODE == TIME_RESERVATION) {
        if(packet->broadcast) {
            for(size_t i = 0; i < port.size(); i++) {
                if((int)i != routing_table[packet->source]) {
                    Packet *newPacket = new Packet(packet);
                    port[i]->sendPacket(newPacket);
                }
            }
            delete packet;
        }
        else {
            port[routing_table[packet->destination]]->sendPacket(packet);
        }
    }
    else if(RESERVATION_MODE == ATS) {
        if(packet->broadcast) {
            for(size_t i = 0; i < port.size(); i++) {
                if((int)i != routing_table[packet->source]) {
                    Packet *newPacket = new Packet(packet);
                    port[i]->sendPacket(newPacket);
                }
            }
            delete packet;
        }
        else {
            packet->arrival_time = _time;
            port[routing_table[packet->destination]]->sendPacket(packet);
        }
    }
    else {
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
}

void Switch::run() {
    for(SWPort *p : port)
        p->run(_time);

    _time++;
}

void Switch::resetTime() {
    _time = 0;
}
