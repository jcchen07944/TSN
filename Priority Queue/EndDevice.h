#ifndef END_DEVICE_H
#define END_DEVICE_H

#include <vector>
#include <utility>

#include "Port.h"
#include "Switch.h"
#include "Packet.h"

class EDPort;
class Switch;

class EndDevice {
public:
    int ID;
    Switch *sw;

    EDPort *port = nullptr;
    double rate;

    EndDevice(int ID);

    ~EndDevice();

    void addNextHop(Switch *sw);

    void sendPacket(Packet* packet);

    void receivePacket(Packet* packet);

    void run();
private:
    long long _time;

    std::vector<std::pair<long long, Packet*>*> _pforward; // Packet forwarding
};

#endif
