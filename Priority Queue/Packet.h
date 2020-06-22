#ifndef PACKET_H
#define PACKET_H

class Packet {
public:
    int p_size; // bit
    int p_priority;
    int p_flow_id;
    int source; // Switch_ID
    int destination; // Switch_ID

    /* For statistic */
    int packet_id;
    int deadline; // microsecond
    long long int send_time;

    Packet();

    ~Packet();

    void run();
private:
    long long _time;
};

#endif
