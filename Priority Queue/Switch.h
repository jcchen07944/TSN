#ifndef SWITCH_H
#define SWITCH_H

#include <vector>
#include <utility>
#include <map>

#include "Flow.h"
#include "Port.h"
#include "EndDevice.h"
#include "Packet.h"

class SWPort;
class EDPort;
class Flow;
class EndDevice;

class Switch {
public:
    int ID;
    double rate; // Mb/s
    std::vector<SWPort*> port;
    std::map<int, int> routing_table; // <MAC, Port>

    /* For Time-reservation */
    std::vector<std::queue<Packet*> > offset_slot; // Reduce number of slots during switch running
    int cycle; // time slot number in a cycle
    std::map<int, int> offset_table; // <flow_id, offset>
    std::map<int, int> time_slot; // <reserved_slot_number, packet_size(bit)>
    std::map<int, Packet*> reserved_table; // <flow_id, flow_info>

    Switch(int ID);

    ~Switch();

    void connectNextHop(int port_num, SWPort *sw_port);

    void connectNextHop(int port_num, EDPort *ed_port);

    SWPort* newPort();

    void receivePacket(int port_num, Packet* packet);

    void run();
private:
    long long _time; // Time in 1/100 microsecond

    long long _accumulate_sequence_number; // For Priority Queue(STL priority_queue is not stable)

    std::vector<std::pair<long long, Packet*>*> _pforward; // Packet forwarding
};

#endif
