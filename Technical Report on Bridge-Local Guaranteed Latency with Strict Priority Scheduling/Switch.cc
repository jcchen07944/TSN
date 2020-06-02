#include <vector>
#include <math.h>
#include <stdio.h>

#include "Switch.h"
#include "Constants.h"

Switch::Switch(int port_count) {
    rate = 1000.0d;

    for(int i = 0; i < port_count; i++)
        reserved_flows.push_back(new std::vector<std::pair<Flow*, int> >());
    port = new Port[port_count];
}

Switch::~Switch() {
    for(int i = 0; i < reserved_flows.size(); i++)
        delete reserved_flows[i];
    delete port;
}

bool Switch::addFlow(Flow *flow) {
    flow->nextHop();
    int output_port = flow->next_route;
    int hop_count = flow->hop_count;
    if(isAccepted(flow)) {
        Switch *next_switch = port[output_port].sw;
        if(next_switch != nullptr) {
            if(!next_switch->addFlow(flow)) {
                return false;
            }
        }
        reserved_flows[output_port]->push_back(std::make_pair(flow, hop_count));
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

    // Higher priority
    for(std::pair<Flow*, int> p : *(reserved_flows[flow->next_route])) {
        Flow *f = p.first;
        int hop_count = p.second;
        if(f->priority > flow->priority) {
            double acc_min_delay = ((double)f->packet_length) / (rate * (double)mega) * ((double)(hop_count - 1));
            double acc_max_delay = guarantee_delay[f->priority] * (double)hop_count;
            latency += (ceil((acc_max_delay - acc_min_delay + guarantee_delay[flow->priority]) / f->burst_interval) *
                        ((double)f->burst_size)/(rate * (double)mega));
        }
    }

    // Equal priority
    for(std::pair<Flow*, int> p : *(reserved_flows[flow->next_route])) {
        Flow *f = p.first;
        int hop_count = p.second;
        if(f->priority == flow->priority) {
            double acc_min_delay = ((double)f->packet_length) / (rate * (double)mega) * ((double)(hop_count - 1));
            double acc_max_delay = guarantee_delay[f->priority] * (double)hop_count;
            latency += (ceil((acc_max_delay - acc_min_delay) / f->burst_interval) *
                        ((double)f->burst_size)/(rate * (double)mega));
        }
    }

    // Lower priority
    double larger_latency = 0.0f;
    for(std::pair<Flow*, int> p : *(reserved_flows[flow->next_route])) {
        Flow *f = p.first;
        if(f->priority < flow->priority)
            larger_latency = std::max(larger_latency, ((double)f->packet_length)/(rate * (double)mega));
    }
    latency += larger_latency;

    printf("%.10f\n", latency);
    return latency;
}

bool Switch::isAccepted(Flow *flow) {
    double worst_latency = computeLatency(flow);
    if(worst_latency > guarantee_delay[flow->priority])
        return false;
    return true;
}
