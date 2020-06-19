#include "EndDevice.h"
#include "Constants.h"

EndDevice::EndDevice(int ID) {
    _time = 0;
    rate = link_speed;
    port = new Port(rate);

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
    // Statistic
    printf("%d\n", _time);
    delete packet;
}

void EndDevice::run() {
    _time++;
}
