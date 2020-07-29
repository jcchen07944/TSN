#include <math.h>
#include <string>

#include "EndDevice.h"
#include "Constants.h"

EndDevice::EndDevice(int ID) {
    _time = 0;
    rate = link_speed;
    port = new EDPort(this, rate);

    this->ID = ID;
    sw_port = nullptr;
    max_latency = 0.0f;
    acc_latency = 0.0f;
    flow_cnt = 0;
    accept_flow = 0;
    reject_flow = 0;
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
    //printf("Flow %d, %lld\n", packet->p_flow_id, _time);
    packet->send_time = _time;
    port->buffer.push(packet);
}

void EndDevice::receivePacket(Packet* packet) {
    if(packet->broadcast) {

    }
    else if(RESERVATION_MODE == TIME_RESERVATION) {
        if(packet->reservation_state == TALKER_ATTRIBUTE) {
            double delay = (packet->acc_slot_count + 1) * slot_duration + (int)floor((double)packet->packet_size / rate / us);
            Packet *new_packet = new Packet(packet);
            new_packet->source = packet->destination;
            new_packet->destination = packet->source;
            new_packet->period = packet->period;
            new_packet->packet_size = packet->packet_size;
            new_packet->acc_slot_count = packet->acc_slot_count;
            new_packet->flow_id = packet->flow_id;
            new_packet->start_transmission_time = packet->start_transmission_time;
            printf("%.4f %.4f\n", delay, packet->deadline);
            if(delay > packet->deadline) {
                new_packet->reservation_state = LISTENER_REJECT;
            }
            else
                new_packet->reservation_state = LISTENER_ACCEPT;
            sendPacket(new_packet);
            delete packet;
            return;
        }
        else if(packet->reservation_state == LISTENER_ACCEPT) {
            // Reserve success
            printf("Flow ID : %d, Accept\n", packet->flow_id);
            accept_flow++;
            delete packet;
            return;
        }
        else if(packet->reservation_state == LISTENER_REJECT) {
            // Reserve failed
            printf("Flow ID : %d, Reject\n", packet->flow_id);
            reject_flow++;
            delete packet;
            return;
        }
    }
    else if(RESERVATION_MODE == ATS) {
        if(packet->reservation_state == TALKER_ATTRIBUTE) {
            Packet *new_packet = new Packet(packet);
            new_packet->source = packet->destination;
            new_packet->destination = packet->source;
            new_packet->period = packet->period;
            new_packet->flow_id = packet->flow_id;
            new_packet->packet_size = packet->packet_size;
            new_packet->acc_max_latency = packet->acc_max_latency;
            printf("%.4f %.4f\n", new_packet->acc_max_latency, packet->deadline);
            if(new_packet->acc_max_latency > packet->deadline)
                new_packet->reservation_state = LISTENER_REJECT;
            else
                new_packet->reservation_state = LISTENER_ACCEPT;
            sendPacket(new_packet);
            delete packet;
            return;
        }
        else if(packet->reservation_state == LISTENER_ACCEPT) {
            // Reserve success
            printf("Flow ID : %d, Accept\n", packet->flow_id);
            accept_flow++;
            delete packet;
            return;
        }
        else if(packet->reservation_state == LISTENER_REJECT) {
            // Reserve failed
            printf("Flow ID : %d, Reject\n", packet->flow_id);
            reject_flow++;
            delete packet;
            return;
        }
    }
    // Statistic
    double latency = (double)(_time - packet->send_time) / 100.0d;
    //printf("EndDevice %d receive flow %d at %.2f, latency : %.2f us\n", ID, packet->p_flow_id, _time / 100.0d, latency);
    if(packet->p_flow_id != -1) {
        max_latency = std::max(max_latency, latency);
        acc_latency += latency;
        flow_cnt++;
        if(write_to_file) {
            std::string mode = RESERVATION_MODE == ATS ? "ats_" : (RESERVATION_MODE == TIME_RESERVATION ? "tr_" : "");
            output_file.open(mode + std::to_string(packet->p_flow_id) + ".txt", std::ios_base::app);
            output_file << latency << std::endl;
            output_file.close();
        }
        //if(packet->deadline < latency) {
        printf("EndDevice %d receive flow %d at %.2f, latency : %.2f us\n", ID, packet->p_flow_id, _time / 100.0d, latency);
        //}
    }
    else {
        if(write_to_file) {
            std::string mode = RESERVATION_MODE == ATS ? "ats_" : (RESERVATION_MODE == TIME_RESERVATION ? "tr_" : "");
            output_file.open(mode + "BE.txt", std::ios_base::app);
            output_file << latency << std::endl;
            output_file.close();
        }
    }

    delete packet;

}

void EndDevice::run() {
    port->run(_time);
    _time++;
}

void EndDevice::resetTime() {
    _time = 0;
}
