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

    sw[1]->setPortNum(2);
    sw[1]->routing_table[1] = 0; // EndDevice 1 -> port 0
    sw[1]->routing_table[2] = 1; // EndDevice 2 -> port 1

    sw[1]->addNextHop(0, nullptr, ed[1]);
    sw[1]->addNextHop(1, nullptr, ed[2]);
    ed[1]->addNextHop(sw[1]);
    ed[2]->addNextHop(sw[1]);

    Packet *BE = new Packet();
    BE->p_size = 1542 * byte;
    BE->p_priority = 0;
    BE->p_flow_id = 0;
    BE->source = 1;
    BE->destination = 2;

    Packet *TSN1 = new Packet();
    TSN1->p_size = 128 * byte;
    TSN1->p_priority = 0;
    TSN1->p_flow_id = 1;
    TSN1->source = 1;
    TSN1->destination = 2;

    Packet *TSN2 = new Packet();
    TSN2->p_size = 64 * byte;
    TSN2->p_priority = 0;
    TSN2->p_flow_id = 2;
    TSN2->source = 1;
    TSN2->destination = 2;

    long long int time = 0;

    while(time < 1000000) { // 0.01 second
        if(time % 100000 == 0)
            ed[1]->sendPacket(BE);

        if(time % 20000 == 0)
            ed[1]->sendPacket(TSN2);

        if(time % 15000 == 0)
            ed[1]->sendPacket(TSN1);

        for(int i = 1; i <= END_DEVICE_COUNT; i++) {
            ed[i]->run();
        }
        for(int i = 1; i <= SWITCH_COUNT; i++) {
            sw[i]->run();
        }

        time++;
    }

    return 0;
}
