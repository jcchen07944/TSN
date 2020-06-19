#ifndef SWITCH_H
#define SWITCH_H

#include <vector>
#include <utility>
#include <map>

#include "Flow.h"
#include "Port.h"
#include "EndDevice.h"
#include "Packet.h"

class Port;
class Flow;
class EndDevice;

class Switch {
public:
    int ID;
    double rate; // Mb/s
    std::vector<Port*> port;
    std::map<int, int> routing_table; // Map Destination to Port

    Switch(int ID);

    Switch(int ID, int port_count);

    ~Switch();

    void addNextHop(int port_num, Switch *sw, EndDevice *ed);

    void setPortNum(int num);

    void recievePacket(Packet* packet);

    void run();
private:
    long long _time; // Time in 1/100 microsecond

    std::vector<std::pair<long long, Packet*>*> _pforward; // Packet forwarding
};

#endif
