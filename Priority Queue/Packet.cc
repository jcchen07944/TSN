#include "Packet.h"

Packet::Packet() {
    p_size = 10;
    p_priority = 0;
    p_flow_id = -1;
    broadcast = false;
}

Packet::Packet(Packet *packet) {
    p_size = packet->p_size;
    p_priority = packet->p_priority;
    p_flow_id = packet->p_flow_id;
    deadline = packet->deadline;
    source = packet->source;
    destination = packet->destination;
    broadcast = packet->broadcast;
}

Packet::~Packet() {

}
