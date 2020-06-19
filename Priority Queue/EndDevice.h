#ifndef END_DEVICE_H
#define END_DEVICE_H

#include "Port.h"
#include "Switch.h"
#include "Packet.h"

class Port;
class Switch;

class EndDevice {
public:
    int ID;
    Switch *sw;

    Port *port = nullptr;
    double rate;

    EndDevice(int ID);

    ~EndDevice();

    void addNextHop(Switch *sw);

    void sendPacket(Packet* packet);

    void recievePacket(Packet* packet);

    void run();
private:
    long long _time;
};

#endif
