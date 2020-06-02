#include <stdio.h>
#include <vector>
#include <math.h>

static const double us = 0.000001;
static const double ms = 0.001;
static const int byte = 8;
static const int mega = 1000000;

class Flow {
public:
    int priority;       // 0~7
    int burst_size;     // bit
    int packet_length;  // bit
    double burst_interval; // second

    Flow() {

    }

    ~Flow() {

    }
};

class Switch {
public:
    double guarantee_delay[8]; // second
    std::vector<Flow*> *reserved_flows;
    double rate = 1000; // Mb/s
    Switch() {
        reserved_flows = new std::vector<Flow*>();
    }

    ~Switch() {
        delete reserved_flows;
    }

    void add_flow(Flow *flow) {
        if(isAccepted(flow))
            reserved_flows->push_back(flow);
    }
private:
    double computeLatency(Flow *flow) {
        double latency = 0.0d;
        double acc_min_delay = ((double)flow->packet_length)/(rate * (double)mega);
        double acc_max_delay = 0.0d;
        for(double delay : guarantee_delay)
            acc_max_delay += delay;

        // Higher priority
        for(Flow *f : *reserved_flows)
            if(f->priority > flow->priority) {
                latency += ((double)f->burst_size)/(rate * (double)mega);
            }
        // Equal priority
        for(Flow *f : *reserved_flows)
            if(f->priority == flow->priority) {
                latency += ((double)f->burst_size)/(rate * (double)mega);
            }
        // Lower priority
        double larger_latency = 0.0f;
        for(Flow *f : *reserved_flows)
            if(f->priority < flow->priority)
                larger_latency = std::max(larger_latency, ((double)f->packet_length)/(rate * (double)mega));
        latency += larger_latency;

        return latency;
    }

    bool isAccepted(Flow *flow) {

        return true;
    }
};

int main() {
    Flow *flow = new Flow();
    flow->priority = 3;
    flow->burst_size = 128 * byte;
    flow->packet_length = 128 * byte;
    flow->burst_interval = 250 * us;

    Switch *sw = new Switch();
    sw->guarantee_delay[0] = 0;
    sw->guarantee_delay[1] = 0;
    sw->guarantee_delay[2] = 250 * us;
    sw->guarantee_delay[3] = 100 * us;
    sw->guarantee_delay[4] = 0;
    sw->guarantee_delay[5] = 0;
    sw->guarantee_delay[6] = 0;
    sw->guarantee_delay[7] = 0;
    sw->add_flow(flow);

    delete sw;
    delete flow;
    return 0;
}
