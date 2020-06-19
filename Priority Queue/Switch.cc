#include <vector>
#include <stdio.h>
#include <math.h>

#include "Switch.h"
#include "Constants.h"

Switch::Switch(int ID) {
    _time = 0;

    this->ID = ID;
    rate = link_speed;
}

Switch::Switch(int ID, int port_count) {
    _time = 0;

    this->ID = ID;
    rate = link_speed;

    setPortNum(port_count);
}

Switch::~Switch() {
    for(int i = 0; i < port.size(); i++) {
        delete port[i];
    }
}

void Switch::addNextHop(int port_num, Switch *sw, EndDevice *ed) {
    if(sw != nullptr) {
        port[port_num]->addDevice(sw);
    }
    else {
        port[port_num]->addDevice(ed);
    }
}

void Switch::setPortNum(int num) {
    for(int i = 0; i < num; i++)
        port.push_back(new Port(rate));
}

void Switch::recievePacket(Packet* packet) {
    std::pair<long long, Packet*> *fpacket = new std::pair<long long, Packet*>();
    fpacket->first = _time + (int)floor((double)packet->p_size / rate / us * 100.0d);
    fpacket->second = packet;
    _pforward.push_back(fpacket);
}

void Switch::run() {
    for(int i = _pforward.size() - 1; i >= 0; i--) {
        std::pair<long long, Packet*> *fpacket = _pforward[i];
        if(_time >= fpacket->first) {
            Packet *packet = fpacket->second;
            port[routing_table[packet->destination]]->t_queue[packet->p_priority]->push(packet);
            _pforward.erase(_pforward.begin() + i);
            delete fpacket;
        }
    }

    for(Port *p : port)
        p->run(_time);

    _time++;
}
