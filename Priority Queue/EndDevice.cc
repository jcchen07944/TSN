#include <math.h>

#include "EndDevice.h"
#include "Constants.h"

EndDevice::EndDevice(int ID) {
    _time = 0;
    rate = link_speed;
    port = new EDPort(this, rate);

    this->ID = ID;
    sw_port = nullptr;
}

EndDevice::~EndDevice() {
    delete port;
}

void EndDevice::connectNextHop(SWPort *sw_port) {
    this->sw_port = sw_port;
    port->addDevice(sw_port);
}

EDPort* EndDevice::newPort() {
    return port;
}

void EndDevice::sendPacket(Packet* packet) {
    packet->send_time = _time;
    port->buffer.push(packet);
}

void EndDevice::receivePacket(Packet* packet) {
    if(packet->broadcast) {

    }
    else if(packet->reservation_state == TALKER_ATTRIBUTE) {
        double delay = (packet->acc_slot_count + 1) * slot_duration;
        Packet *new_packet = new Packet(packet);
        new_packet->source = packet->destination;
        new_packet->destination = packet->source;
        new_packet->period = packet->period;
        new_packet->packet_size = packet->packet_size;
        new_packet->acc_slot_count = packet->acc_slot_count;
        new_packet->start_transmission_time = packet->start_transmission_time;
        if(delay > packet->deadline)
            new_packet->reservation_state = LISTENER_REJECT;
        else
            new_packet->reservation_state = LISTENER_ACCEPT;
        sendPacket(new_packet);
    }
    else if(packet->reservation_state == LISTENER_ACCEPT) {
        // Reserve success
        printf("Flow ID : %d, Accept\n", packet->p_flow_id);
    }
    else if(packet->reservation_state == LISTENER_REJECT) {
        // Reserve failed
        printf("Flow ID : %d, Reject\n", packet->p_flow_id);
    }
    else {
        // Statistic
        double latency = (double)(_time - packet->send_time) / 100.0d;
        //printf("EndDevice %d receive flow %d at %.2f, latency : %.2f us\n", ID, packet->p_flow_id, _time / 100.0d, latency);
        //if(packet->p_flow_id == 0)
            if(packet->deadline < latency) {
                printf("EndDevice %d receive flow %d at %.2f, latency : %.2f us\n", ID, packet->p_flow_id, (_time / 100.0d), latency);
            }
    }

    delete packet;
}

void EndDevice::run() {
    port->run(_time);
    _time++;
}
