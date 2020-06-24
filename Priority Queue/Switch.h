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
    std::map<int, int> routing_table; // Map Mac Address to Port

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
