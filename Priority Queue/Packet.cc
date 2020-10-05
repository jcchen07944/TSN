#include "Packet.h"

Packet::Packet() {
    p_size = 100;
    p_priority = 0;
    p_flow_id = -1;
    broadcast = false;
    reservation_state = -1;
    acc_slot_count = 0;
    acc_max_latency = 0.0f;
    hop_count = 0;
}

Packet::Packet(Packet *packet) {
    p_size = packet->p_size;
    p_priority = packet->p_priority;
    p_flow_id = packet->p_flow_id;
    flow_id = packet->flow_id;
    deadline = packet->deadline;
    per_hop_deadline = packet->per_hop_deadline;
    priority = packet->priority;
    source = packet->source;
    destination = packet->destination;
    broadcast = packet->broadcast;
    reservation_state = packet->reservation_state;
    hop_count = packet->hop_count;
}

Packet::~Packet() {

}
