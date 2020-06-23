#include "Packet.h"

Packet::Packet() {
    p_size = 0;
    p_priority = 0;
    p_flow_id = -1;
    broadcast = false;
}

Packet::~Packet() {

}
