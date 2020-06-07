#include <vector>
#include <stdio.h>

#include "Switch.h"
#include "Constants.h"

Switch::Switch(int ID, int port_count) {
    this->ID = ID;
    rate = 1000.0d * (double)mega; // Mb/s

    port = new Port[port_count];
    for(int i = 0; i < port_count; i++) {
        port[i].guarantee_delay = guarantee_delay;
        port[i].rate = rate;
    }
}

Switch::~Switch() {
    delete port;
}

bool Switch::addFlow(Flow *flow) {
    flow->nextHop();
    int output_port = flow->next_route;
    printf("Switch %d, Port %d\n", ID, output_port);
    return port[output_port].addFlow(flow);
}

void Switch::addNextHop(int port_num, Switch *sw, EndDevice *ed) {
    if(sw != nullptr) {
        port[port_num].addDevice(sw);
    }
    else {
        port[port_num].addDevice(ed);
    }
}
