#ifndef FLOW_H
#define FLOW_H

#include "EndDevice.h"

class EndDevice;

class Flow {
public:
    int ID;
    int priority;       // 0~7
    int burst_size;     // bit
    int packet_size;  // bit
    double period; // microsecond
    double deadline; // microsecond
    int source; // Mac Address
    int destination; // Mac Address
    int start_time;
    int hop_count; // For ATS reservation

    /* For packet information */
    int packet_id;

    Flow(int ID);

    ~Flow();

    void run(EndDevice *ed, int flow_type);
private:
    /* For packet sending */
    long long _next_packet_time;

    long long _time;
};

#endif
