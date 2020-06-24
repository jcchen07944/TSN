#ifndef PACKET_H
#define PACKET_H

class Packet {
public:
    int p_size; // bit
    int p_priority;
    int p_flow_id;
    int source; // Mac Address
    int destination; // Mac Address

    /* For statistic */
    int packet_id;
    int deadline; // microsecond
    long long send_time;

    /* For Mac Address Table */
    bool broadcast;

    /* For Priority Queue(STL priority_queue is not stable) */
    long long sequence_number;

    Packet();

    Packet(Packet *packet);

    ~Packet();

    void run();
private:
    long long _time;
};

#endif
