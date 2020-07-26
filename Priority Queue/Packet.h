#ifndef PACKET_H
#define PACKET_H

class Packet {
public:
    int p_size; // bit
    int p_priority;
    int p_flow_id;
    int source; // Mac Address
    int destination; // Mac Address

    long long arrival_time;

    /* For statistic */
    int packet_id;
    long long send_time; // microsecond

    /* For Mac Address Table */
    bool broadcast;

    /* For Priority Queue(STL priority_queue is not stable) */
    long long sequence_number;

    /*****************/
    /** Reservation **/
    /*****************/
    int reservation_state; // TALKER_ATTRIBUTE, LISTENER_ACCEPT, LISTENER_REJECT
    int packet_size; // Value for talker attribute, not for transmission. (bits)
    double period; // microsecond
    double deadline; // microsecond
    /* For Time Reservation */
    double start_transmission_time; // microsecond
    int acc_slot_count;
    int flow_id;
    /* For Asynchronous Traffic Shaper */
    double acc_max_latency;

    Packet();

    Packet(Packet *packet);

    ~Packet();

    void run();
private:
    long long _time;
};

#endif
