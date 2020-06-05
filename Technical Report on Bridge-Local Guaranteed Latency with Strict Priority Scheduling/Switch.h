#ifndef SWITCH_H
#define SWITCH_H

#include <vector>
#include <utility>

#include "Flow.h"
#include "Port.h"
#include "EndDevice.h"

class Port;
class Flow;
class EndDevice;

class Switch {
public:

    int ID;
    double guarantee_delay[8]; // second
    double rate; // Mb/s
    Port *port;

    Switch(int ID, int port_count);

    ~Switch();

    bool addFlow(Flow *flow);

    void addNextHop(int port_num, Switch *sw, EndDevice *ed);

private:
};

#endif
