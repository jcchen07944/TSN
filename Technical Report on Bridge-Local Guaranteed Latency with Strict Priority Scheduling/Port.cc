#include "Port.h"

Port::Port() {
    device = 0;
    sw = nullptr;
    ed = nullptr;
}

Port::~Port() {
    delete sw;
    delete ed;
}

void Port::addDevice(Switch *sw) {
    this->sw = sw;
    device = SWITCH;
}

void Port::addDevice(EndDevice *ed) {
    this->ed = ed;
    device = END_DEVICE;
}

