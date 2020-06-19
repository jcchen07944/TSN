#include "Packet.h"

Packet::Packet() {
    p_size = 42;
    p_priority = 0;
    p_flow_id = -1;
}

Packet::~Packet() {

}
