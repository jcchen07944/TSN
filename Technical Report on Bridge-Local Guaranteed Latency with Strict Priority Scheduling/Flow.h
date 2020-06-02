#ifndef FLOW_H
#define FLOW_H

class Flow {
public:
    int ID;
    int priority;       // 0~7
    int burst_size;     // bit
    int packet_length;  // bit
    double burst_interval; // second

    int max_hop = 0;
    int *route;
    int next_route = 0;

    Flow(int ID);

    ~Flow();

    bool nextHop();

    void restartRoute();
private:
    int _next_route_index = 0;
};

#endif
