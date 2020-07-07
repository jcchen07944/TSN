#ifndef END_DEVICE_H
#define END_DEVICE_H

#include <vector>
#include <utility>

#include "Port.h"
#include "Packet.h"

class EDPort;
class SWPort;

class EndDevice {
public:
    int ID;
    SWPort *sw_port;

    EDPort *port = nullptr;
    double rate;

    double max_latency;
    double acc_latency;
    int flow_cnt;

    EndDevice(int ID);

    ~EndDevice();

    void connectNextHop(SWPort *sw_port);

    EDPort* newPort();

    void sendPacket(Packet* packet);

    void receivePacket(Packet* packet);

    void run();
private:
    long long _time;


    std::vector<std::pair<long long, Packet*>*> _pforward; // Packet forwarding
};

#endif
