#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"
#include "Packet.h"


int main() {
    const int END_DEVICE_COUNT = 2;
    const int SWITCH_COUNT = 1;

    EndDevice *ed[END_DEVICE_COUNT + 1];
    for(int i = 1; i <= END_DEVICE_COUNT; i++) {
        ed[i] = new EndDevice(i);
    }

    Switch *sw[SWITCH_COUNT + 1];
    for(int i = 1; i <= SWITCH_COUNT; i++) {
        sw[i] = new Switch(i + END_DEVICE_COUNT);
    }

    sw[1]->routing_table[1] = 0; // EndDevice 1 -> port 0
    sw[1]->routing_table[2] = 1; // EndDevice 2 -> port 1

    sw[1]->addNextHop(0, ed[1]);
    sw[1]->addNextHop(1, ed[2]);

    ed[1]->addNextHop(sw[1]);
    ed[2]->addNextHop(sw[1]);



    return 0;
}
