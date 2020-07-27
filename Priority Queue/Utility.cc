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

void Utility::setupTSN(Flow *TSN, double period, int packet_size, int source, int destination, int start_time) {
    TSN->deadline = TSN->period = period;
    TSN->packet_size = TSN->burst_size = packet_size * byte;
    TSN->source = source;
    TSN->destination = destination;
    TSN->priority = 7;
    TSN->start_time = start_time;
}

void Utility::setupTSN(Flow *TSN, double period, double deadline, int packet_size, int source, int destination, int start_time) {
    TSN->deadline = deadline;
    TSN->period = period;
    TSN->packet_size = TSN->burst_size = packet_size * byte;
    TSN->source = source;
    TSN->destination = destination;
    TSN->priority = 7;
    TSN->start_time = start_time;
}

void Utility::reserveTSN(Flow *TSN, std::vector<Switch*> sw, std::vector<EndDevice*> ed) {
    if(RESERVATION_MODE != TIME_RESERVATION && RESERVATION_MODE != ATS)
        return;
    Packet *talker_attribute = new Packet();
    talker_attribute->reservation_state = TALKER_ATTRIBUTE;
    talker_attribute->p_size = 42 * byte;
    talker_attribute->p_flow_id = -1;
    talker_attribute->source = TSN->source;
    talker_attribute->destination = TSN->destination;
    talker_attribute->packet_size = TSN->packet_size;
    talker_attribute->period = TSN->period;
    talker_attribute->deadline = TSN->deadline;
    talker_attribute->priority = 7;
    if(RESERVATION_MODE == ATS)
        talker_attribute->per_hop_deadline = TSN->deadline / hop_count;
    talker_attribute->start_transmission_time = TSN->start_time;
    talker_attribute->flow_id = TSN->ID;
    talker_attribute->p_priority = 0;

    ed[TSN->source]->sendPacket(talker_attribute);

    long long int time = 0;
    while(time++ < 10000) {
        for(size_t i = 0; i < ed.size(); i++)
            ed[i]->run();
        for(size_t i = 0; i < sw.size(); i++)
            sw[i]->run();
    }
}

void Utility::setupAVB(Flow *AVB, char SRClass, int packet_size, int source, int destination, int start_time) {
    if(SRClass == 'A' || SRClass == 'a') {
        AVB->deadline = 2000;
        AVB->period = 125;
        AVB->priority = 5;
    }
    else if(SRClass == 'B' || SRClass == 'b') {
        AVB->deadline = 50000;
        AVB->period = 250;
        AVB->priority = 6;
    }
    AVB->packet_size = packet_size * byte;
    AVB->source = source;
    AVB->destination = destination;
    AVB->start_time = start_time;
}

void Utility::setupBE(Flow *BE, int packet_size, int source, int destination) {
    BE->priority = 0;
    BE->packet_size = packet_size * byte;
    BE->source = source;
    BE->destination = destination;
}

int Utility::gcd(int m, int n) {
    while(n != 0) {
        int r = m % n;
        m = n;
        n = r;
    }
    return m;
}

int Utility::lcm(int m, int n) {
    return m * n / gcd(m, n);
}

void Utility::resetNetworkTime(std::vector<Switch*> sw, std::vector<EndDevice*> ed) {
    for(size_t i = 0; i < ed.size(); i++)
        ed[i]->resetTime();
    for(size_t i = 0; i < sw.size(); i++)
        sw[i]->resetTime();
}
