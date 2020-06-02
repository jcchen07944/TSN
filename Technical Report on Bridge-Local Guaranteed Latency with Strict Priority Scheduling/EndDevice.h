#ifndef END_DEVICE_H
#define END_DEVICE_H

#include "Port.h"
#include "Switch.h"

class Port;
class Switch;

class EndDevice {
public:
    Port *port = nullptr;

    EndDevice();

    ~EndDevice();

    void addNextHop(Switch *sw);
};

#endif
