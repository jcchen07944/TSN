#ifndef PORT_H
#define PORT_H

#include "Switch.h"
#include "EndDevice.h"

class Port {
public:
    int device;
    Switch *sw;
    EndDevice *ed;

    Port();

    ~Port();

    void addDevice(Switch *sw);

    void addDevice(EndDevice *ed);
private:
    int SWITCH = 1;
    int END_DEVICE = 2;
};

#endif
