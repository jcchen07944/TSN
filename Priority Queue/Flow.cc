#include <time.h>

#include "Flow.h"
#include "Packet.h"

Flow::Flow(int ID) {
    this->ID = ID;
    this->_next_packet_time = 0;
    this->_time = 0;
}

Flow::~Flow() {

}

void Flow::run(EndDevice *ed) {
    srand(time(NULL));
    if(_time >= (_next_packet_time + start_time * 100)) {
        _next_packet_time += period * 100;
        Packet *packet = new Packet();
        packet->deadline = deadline;
        packet->source = source;
        packet->destination = destination;
        packet->packet_id = packet_id;
        packet->p_size = packet_size;
        packet->p_priority = priority;
        packet->broadcast = false;
        packet->p_flow_id = ID;
        ed->sendPacket(packet);
    }
    _time++;
}

