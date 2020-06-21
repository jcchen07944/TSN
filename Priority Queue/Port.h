#ifndef PORT_H
#define PORT_H

#include <vector>
#include <queue>

#include "Switch.h"
#include "EndDevice.h"

class Switch;
class EndDevice;

class Port {
public:
    int device;
    Switch *sw;
    EndDevice *ed;
    double rate;

    Port();

    ~Port();

    void addDevice(Switch *sw);

    void addDevice(EndDevice *ed);
protected:
    int SWITCH = 1;
    int END_DEVICE = 2;
};

class SWPort : public Port {
public:
    std::vector<std::queue<Packet*>*> t_queue;

    SWPort(double rate);

    ~SWPort();

    void run(long long time);
private:
    Packet *_pforward; // Packet forwarding
    long long _tforward; // Forwarding time
};

class EDPort : public Port {
public:
    EDPort(double rate);

    ~EDPort();

    void run(long long time);
private:
    Packet *_pforward; // Packet forwarding
    long long _tforward; // Forwarding time
};

#endif
