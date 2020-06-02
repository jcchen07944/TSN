#include <vector>
#include <math.h>
#include <stdio.h>

#include "Switch.h"
#include "Constants.h"

Switch::Switch(int port_count) {
    for(int i = 0; i < port_count; i++)
        reserved_flows.push_back(new std::vector<Flow*>());
    port = new Port[port_count];
}

Switch::~Switch() {
    for(int i = 0; i < reserved_flows.size(); i++)
        delete reserved_flows[i];
    delete port;
}

bool Switch::addFlow(Flow *flow) {
    printf("%d\n", flow->next_route);
    if(isAccepted(flow)) {
        if(flow->nextHop()) {
            if(!port[flow->next_route].sw->addFlow(flow)) {
                return false;
            }
        }
        reserved_flows[flow->next_route]->push_back(flow);
        //printf("");
        return true;
    }
    else {
        return false;
    }

}

void Switch::addNextHop(int port_num, Switch *sw, EndDevice *ed) {
    if(sw != nullptr) {
        port[port_num].addDevice(sw);
    }
    else {
        port[port_num].addDevice(ed);
    }
}

double Switch::computeLatency(Flow *flow) {
    double latency = 0.0d;
    printf("%f\n", rate * (double)mega);
    double acc_min_delay = ((double)flow->packet_length)/(rate * (double)mega);
    double acc_max_delay = 0.0d;
    for(double delay : guarantee_delay)
        acc_max_delay += delay;

    // Higher priority
    for(Flow *f : *(reserved_flows[flow->next_route])) {
        if(f->priority > flow->priority) {
            latency += ceil((acc_max_delay - acc_min_delay + guarantee_delay[flow->priority]) / flow->burst_interval) *
                        ((double)f->burst_size)/(rate * (double)mega);
        }
    }

    // Equal priority
    for(Flow *f : *(reserved_flows[flow->next_route])) {
        if(f->priority == flow->priority) {
            latency += ceil((acc_max_delay - acc_min_delay) / flow->burst_interval) *
                        ((double)f->burst_size)/(rate * (double)mega);
        }
    }

    // Lower priority
    double larger_latency = 0.0f;
    for(Flow *f : *(reserved_flows[flow->next_route]))
        if(f->priority < flow->priority)
            larger_latency = std::max(larger_latency, ((double)f->packet_length)/(rate * (double)mega));
    latency += larger_latency;

    return latency;
}

bool Switch::isAccepted(Flow *flow) {
    double worst_latency = computeLatency(flow);
    if(worst_latency > guarantee_delay[flow->priority])
        return false;
    return true;
}
