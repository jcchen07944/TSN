#ifndef FLOW_H
#define FLOW_H

class Flow {
public:
    int ID;
    int priority;       // 0~7
    int burst_size;     // bit
    int packet_length;  // bit
    double burst_interval; // second

    int *route;
    int next_route = 0;
    int hop_count = 0;

    Flow(int ID);

    ~Flow();

    void nextHop();

    void restartRoute();
private:
};

#endif
