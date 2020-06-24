#include "Utility.h"
#include "Port.h"
#include "Constants.h"

Utility::Utility() {

}

Utility::~Utility() {

}

void Utility::connectToSwitch(Switch *sw1, Switch *sw2) {
    SWPort *sw_port1 = sw1->newPort();
    SWPort *sw_port2 = sw2->newPort();
    sw1->connectNextHop(sw_port1->port_num, sw_port2);
    sw2->connectNextHop(sw_port2->port_num, sw_port1);
}

void Utility::connectToSwitch(Switch *sw, EndDevice *ed) {
    EDPort *ed_port = ed->newPort();
    SWPort *sw_port = sw->newPort();
    sw->connectNextHop(sw_port->port_num, ed_port);
    ed->connectNextHop(sw_port);
}

void Utility::broadcastEndDevice(std::vector<Switch*> sw, std::vector<EndDevice*> ed) {
    for(size_t i = 0; i < ed.size(); i++) {
        Packet *broadcast_packet = new Packet();
        broadcast_packet->source = ed[i]->ID;
        broadcast_packet->broadcast = true;
        ed[i]->sendPacket(broadcast_packet);

    }
    long long int time = 0;
        while(time++ < 10000) {
            for(size_t i = 0; i < ed.size(); i++)
                ed[i]->run();
            for(size_t i = 0; i < sw.size(); i++)
                sw[i]->run();
        }

    // Debug
    for(size_t i = 0; i < sw.size(); i++) {
        printf("Switch ID : %d\n", (int)i);
        for(auto it = sw[i]->routing_table.cbegin(); it != sw[i]->routing_table.cend(); it++) {
            printf("Destination %d -> Port %d\n", it->first, it->second);
        }
    }
}
