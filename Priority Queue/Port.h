#ifndef PORT_H
#define PORT_H

#include <vector>
#include <queue>

#include "Switch.h"
#include "EndDevice.h"

class Port {
public:
    int device;
    Switch *sw;
    EndDevice *ed;
    double rate;
    std::vector<std::queue<Packet*>*> t_queue;

    Port(double rate);

    ~Port();

    void addDevice(Switch *sw);

    void addDevice(EndDevice *ed);

    void run(long long time);
private:
    int SWITCH = 1;
    int END_DEVICE = 2;

    Packet *_pforward; // Packet forwarding
    long long _tforward; // Forwarding time
};

#endif
