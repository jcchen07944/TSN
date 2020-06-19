#ifndef PACKET_H
#define PACKET_H

class Packet {
public:
    int p_size; // bit
    int p_priority;
    int p_flow_id;
    int source; // Switch_ID
    int destination; // Switch_ID

    int packet_id; // For statistic

    Packet();

    ~Packet();

    void run();
private:
    long long _time;
};

#endif
