#include <math.h>
#include <stdio.h>

#include "Port.h"
#include "Constants.h"

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

double Port::computeLatency(Flow *flow, int MODE) {
    double latency = 0.0d;
    double larger_latency = 0.0f;

    /*
    ** Use Strict Priority method.
    */
    if(MODE == SP_MODE) {
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
    }
    /*
    ** Use Urgency-Based Scheduler method. (LRQ)
    */
    else if(MODE == UBS_MODE) {
        for(std::pair<Flow*, Accumulate*> p : reserved_flows) {
            Flow *j = p.first;
            if(j->priority != flow->priority)
                continue;
            bool has_low_packet = false;
            double burst_sum = 0.0f;
            double rate_sum = 0.0f;
            for(std::pair<Flow*, Accumulate*> q : reserved_flows) {
                Flow *f = q.first;

                if(f->priority > j->priority) {
                    burst_sum += f->burst_size;
                    rate_sum += f->packet_length / f->burst_interval;
                }
                else if(f->priority == flow->priority) {
                    if(f->ID != j->ID)
                        burst_sum += f->burst_size;
                }
                else {
                    has_low_packet = true;
                }
            }
            latency = std::max(latency, (burst_sum + (has_low_packet? (1542 * byte):0)) / (rate - rate_sum) + (j->packet_length / rate));
        }
    }

    //printf("%.9f\n", latency);
    return latency;
}

bool Port::isAccepted(Flow *flow) {
    Accumulate *accu = new Accumulate();
    accu->max_delay = guarantee_delay[flow->priority] * (double)flow->hop_count;
    accu->min_delay = (flow->packet_length / rate) * (double)(flow->hop_count - 1);
    reserved_flows.push_back(std::make_pair(flow, accu));

    for(std::pair<Flow*, Accumulate*> p : reserved_flows) {
        Flow *f = p.first;
        double latency = computeLatency(f, UBS_MODE);
        double deadline;
        if(f->priority == 5)
            deadline = 2 * ms;
        else if(f->priority == 7) {
            deadline = f->burst_interval;
        }
        else {
            deadline = 1000 * ms;
        }
        //if(f->ID == 11)
            printf("bound : %.9f, deadline : %.9f\n", latency, deadline);
        if(latency > deadline) {
            delete accu;
            reserved_flows.pop_back();
            return false;
        }
    }
    delete accu;
    reserved_flows.pop_back();
    return true;
}

