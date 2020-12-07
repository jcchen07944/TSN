#include <math.h>
#include <stdio.h>

#include "Port.h"
#include "Constants.h"

Port::Port() {
    device = 0;
    sw_port = nullptr;
    ed_port = nullptr;
}

Port::~Port() {
    delete sw_port;
    delete ed_port;
}

void Port::addDevice(SWPort *sw_port) {
    this->sw_port = sw_port;
    device = SWITCH;
}

void Port::addDevice(EDPort *ed_port) {
    this->ed_port = ed_port;
    device = END_DEVICE;
}
