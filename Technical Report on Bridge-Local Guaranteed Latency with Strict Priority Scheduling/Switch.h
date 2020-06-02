#ifndef SWITCH_H
#define SWITCH_H

#include <vector>

#include "Flow.h"
#include "Port.h"
#include "EndDevice.h"

class Port;
class Flow;
class EndDevice;

class Switch {
public:
    double guarantee_delay[8]; // second
    std::vector<std::vector<Flow*>*> reserved_flows;
    double rate = 1000; // Mb/s
    Port *port;

    Switch(int port_count);

    ~Switch();

    bool addFlow(Flow *flow);

    void addNextHop(int port_num, Switch *sw, EndDevice *ed);

private:
    double computeLatency(Flow *flow);

    bool isAccepted(Flow *flow);
};

#endif
