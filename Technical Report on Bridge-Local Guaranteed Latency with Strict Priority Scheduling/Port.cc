#include <math.h>
#include <stdio.h>

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

bool Port::addFlow(Flow* flow) {
    if(isAccepted(flow)) {
        if(sw != nullptr) {
            if(!sw->addFlow(flow)) {
                return false;
            }
        }
        Accumulate *accu = new Accumulate();
        accu->max_delay = guarantee_delay[flow->priority] * (double)flow->hop_count;
        accu->min_delay = (flow->packet_length / rate) * (double)(flow->hop_count - 1);
        reserved_flows.push_back(std::make_pair(flow, accu));
        return true;
    }
    else {
        return false;
    }
}

double Port::computeLatency(Flow *flow) {
    double latency = 0.0d;
    double larger_latency = 0.0f;

    for(std::pair<Flow*, Accumulate*> p : reserved_flows) {
        Flow *f = p.first;
        if(f->ID == flow->ID)
            continue;

        Accumulate *accu = p.second;

        // Higher priority
        if(f->priority > flow->priority) {
            latency += (ceil((accu->max_delay - accu->min_delay + guarantee_delay[flow->priority]) / f->burst_interval) *
                        ((double)f->burst_size) / rate);
        }
        // Equal priority
        else if(f->priority == flow->priority) {
            latency += (ceil((accu->max_delay - accu->min_delay) / f->burst_interval) *
                        ((double)f->burst_size) / rate);
        }
        // Lower priority
        else {
            larger_latency = std::max(larger_latency, ((double)f->packet_length)/rate);
        }
    }
    latency += larger_latency;

    // printf("%.9f\n", latency);
    return latency;
}

bool Port::isAccepted(Flow *flow) {
    Accumulate *accu = new Accumulate();
    accu->max_delay = guarantee_delay[flow->priority] * (double)flow->hop_count;
    accu->min_delay = (flow->packet_length / rate) * (double)(flow->hop_count - 1);
    reserved_flows.push_back(std::make_pair(flow, accu));

    for(std::pair<Flow*, Accumulate*> p : reserved_flows) {
        Flow *f = p.first;
        double latency = computeLatency(f);
        if(f->ID == 2)
            printf("%.9f\n", latency);
        if(latency > guarantee_delay[f->priority]) {
            delete accu;
            reserved_flows.pop_back();
            return false;
        }
    }
    delete accu;
    reserved_flows.pop_back();
    return true;
}

