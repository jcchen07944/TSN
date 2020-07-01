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
    double deadline; // microsecond
    long long send_time; // microsecond

    /* For Mac Address Table */
    bool broadcast;

    /* For Priority Queue(STL priority_queue is not stable) */
    long long sequence_number;

    /* For Time-reservation */
    bool talker_attribute;
    double period; // microsecond
    int acc_hop_count;

    Packet();

    Packet(Packet *packet);

    ~Packet();

    void run();
private:
    long long _time;
};

#endif
