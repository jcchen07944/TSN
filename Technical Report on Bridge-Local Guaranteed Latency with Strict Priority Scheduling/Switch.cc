#include <vector>
#include <math.h>
#include <stdio.h>

#include "Switch.h"
#include "Constants.h"

Switch::Switch(int ID, int port_count) {
    this->ID = ID;
    rate = 1000.0d * (double)mega; // Mb/s

    for(int i = 0; i < port_count; i++)
        reserved_flows.push_back(new std::vector<std::pair<Flow*, Accumulate*> >());
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
    printf("Switch %d, Port %d\n", ID, output_port);
    if(isAccepted(flow)) {
        Switch *next_switch = port[output_port].sw;
        if(next_switch != nullptr) {
            if(!next_switch->addFlow(flow)) {
                return false;
            }
        }
        Accumulate *accu = new Accumulate();
        accu->max_delay = guarantee_delay[flow->priority] * (double)hop_count;
        accu->min_delay = (flow->packet_length / rate) * (double)(hop_count - 1);
        reserved_flows[output_port]->push_back(std::make_pair(flow, accu));
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
    for(std::pair<Flow*, Accumulate*> p : *(reserved_flows[flow->next_route])) {
        Flow *f = p.first;
        Accumulate *accu = p.second;
        if(f->priority > flow->priority) {
            latency += (ceil((accu->max_delay - accu->min_delay + guarantee_delay[flow->priority]) / f->burst_interval) *
                        ((double)f->burst_size)/rate);
        }
    }

    // Equal priority
    for(std::pair<Flow*, Accumulate*> p : *(reserved_flows[flow->next_route])) {
        Flow *f = p.first;
        Accumulate *accu = p.second;
        if(f->priority == flow->priority) {
            latency += (ceil((accu->max_delay - accu->min_delay) / f->burst_interval) *
                        ((double)f->burst_size)/rate);
        }
    }

    // Lower priority
    double larger_latency = 0.0f;
    for(std::pair<Flow*, Accumulate*> p : *(reserved_flows[flow->next_route])) {
        Flow *f = p.first;
        if(f->priority < flow->priority)
            larger_latency = std::max(larger_latency, ((double)f->packet_length)/rate);
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
