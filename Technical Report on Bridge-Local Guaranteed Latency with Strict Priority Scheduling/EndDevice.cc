#include "EndDevice.h"

EndDevice::EndDevice() {
    port = new Port();
}

EndDevice::~EndDevice() {
    delete port;
}

void EndDevice::addNextHop(Switch *sw) {
    port->addDevice(sw);
}
