#include <math.h>

#include "EndDevice.h"
#include "Constants.h"

EndDevice::EndDevice(int ID) {
    _time = 0;
    rate = link_speed;
    port = new EDPort(rate);

    this->ID = ID;
    sw = nullptr;
}

EndDevice::~EndDevice() {
    delete port;
}

void EndDevice::addNextHop(Switch *sw) {
    this->sw = sw;
    port->addDevice(sw);
}

void EndDevice::sendPacket(Packet* packet) {
    sw->recievePacket(packet);

}

void EndDevice::recievePacket(Packet* packet) {
    std::pair<long long, Packet*> *fpacket = new std::pair<long long, Packet*>();
    fpacket->first = _time + (int)floor((double)packet->p_size / rate / us * 100.0d);
    fpacket->second = packet;
    _pforward.push_back(fpacket);
}

void EndDevice::run() {
    for(int i = _pforward.size() - 1; i >= 0; i--) {
        std::pair<long long, Packet*> *fpacket = _pforward[i];
        if(_time >= fpacket->first) {
            Packet *packet = fpacket->second;

            // Statistic
            printf("%d\n", _time);
            delete packet;

            _pforward.erase(_pforward.begin() + i);
            delete fpacket;
        }
    }

    _time++;
}
