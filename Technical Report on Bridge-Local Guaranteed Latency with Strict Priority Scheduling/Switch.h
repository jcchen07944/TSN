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
    struct Accumulate {
        double max_delay;
        double min_delay;
    };

    int ID;
    double guarantee_delay[8]; // second
    std::vector<std::vector<std::pair<Flow*, Accumulate*> >*> reserved_flows;
    double rate; // Mb/s
    Port *port;

    Switch(int ID, int port_count);

    ~Switch();

    bool addFlow(Flow *flow);

    void addNextHop(int port_num, Switch *sw, EndDevice *ed);

private:
    double computeLatency(Flow *flow);

    bool isAccepted(Flow *flow);
};

#endif
