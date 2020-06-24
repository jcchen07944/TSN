#ifndef FLOW_H
#define FLOW_H

class Flow {
public:
    int ID;
    int priority;       // 0~7
    int burst_size;     // bit
    int packet_size;  // bit
    double period; // second
    double deadline;
    int source; // Mac Address
    int destination; // Mac Address

    Flow(int ID);

    ~Flow();

private:
};

#endif
