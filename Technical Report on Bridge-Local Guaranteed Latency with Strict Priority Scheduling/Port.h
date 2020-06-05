#ifndef PORT_H
#define PORT_H

#include <vector>

#include "Switch.h"
#include "EndDevice.h"

class Port {
public:
    struct Accumulate {
        double max_delay;
        double min_delay;
    };

    int device;
    Switch *sw;
    EndDevice *ed;
    double rate;

    double *guarantee_delay;
    std::vector<std::pair<Flow*, Accumulate*> > reserved_flows;

    Port();

    ~Port();

    void addDevice(Switch *sw);

    void addDevice(EndDevice *ed);

    bool addFlow(Flow *flow);
private:
    int SWITCH = 1;
    int END_DEVICE = 2;

    double computeLatency(Flow *flow);

    bool isAccepted(Flow *flow);
};

#endif
