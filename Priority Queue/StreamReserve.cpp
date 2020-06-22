#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"
#include "Packet.h"

int main() {
    const int END_DEVICE_COUNT = 13;
    const int SWITCH_COUNT = 1;

    EndDevice *ed[END_DEVICE_COUNT + 1];
    for(int i = 1; i <= END_DEVICE_COUNT; i++) {
        ed[i] = new EndDevice(i);
    }

    Switch *sw[SWITCH_COUNT + 1];
    for(int i = 1; i <= SWITCH_COUNT; i++) {
        sw[i] = new Switch(i + END_DEVICE_COUNT);
    }

    sw[1]->setPortNum(13);
    sw[1]->routing_table[1] = 0; // EndDevice 1 -> port 0
    sw[1]->routing_table[2] = 1; // EndDevice 2 -> port 1
    sw[1]->routing_table[3] = 2; // EndDevice 3 -> port 2
    sw[1]->routing_table[4] = 3; // EndDevice 4 -> port 3
    sw[1]->routing_table[5] = 4; // EndDevice 5 -> port 4
    sw[1]->routing_table[6] = 5; // EndDevice 6 -> port 5
    sw[1]->routing_table[7] = 6; // EndDevice 7 -> port 6
    sw[1]->routing_table[8] = 7; // EndDevice 8 -> port 7
    sw[1]->routing_table[9] = 8; // EndDevice 9 -> port 8
    sw[1]->routing_table[10] = 9; // EndDevice 10 -> port 9
    sw[1]->routing_table[11] = 10; // EndDevice 11 -> port 10
    sw[1]->routing_table[12] = 11; // EndDevice 12 -> port 11
    sw[1]->routing_table[13] = 12; // EndDevice 13 -> port 12

    sw[1]->addNextHop(0, ed[1]);
    sw[1]->addNextHop(1, ed[2]);
    sw[1]->addNextHop(2, ed[3]);
    sw[1]->addNextHop(3, ed[4]);
    sw[1]->addNextHop(4, ed[5]);
    sw[1]->addNextHop(5, ed[6]);
    sw[1]->addNextHop(6, ed[7]);
    sw[1]->addNextHop(7, ed[8]);
    sw[1]->addNextHop(8, ed[9]);
    sw[1]->addNextHop(9, ed[10]);
    sw[1]->addNextHop(10, ed[11]);
    sw[1]->addNextHop(11, ed[12]);
    sw[1]->addNextHop(12, ed[13]);
    ed[1]->addNextHop(sw[1]);
    ed[2]->addNextHop(sw[1]);
    ed[3]->addNextHop(sw[1]);
    ed[4]->addNextHop(sw[1]);
    ed[5]->addNextHop(sw[1]);
    ed[6]->addNextHop(sw[1]);
    ed[7]->addNextHop(sw[1]);
    ed[8]->addNextHop(sw[1]);
    ed[9]->addNextHop(sw[1]);
    ed[10]->addNextHop(sw[1]);
    ed[11]->addNextHop(sw[1]);
    ed[12]->addNextHop(sw[1]);
    ed[13]->addNextHop(sw[1]);

    srand(time(NULL));

    long long int time = 0;

    long long int next_packet_send_time[12] = {rand()%1000};
    int packet_id[12] = {1};

    while(time < 100000000) { // 1 second
        if(time == next_packet_send_time[0]) {
            Packet *BE = new Packet();
            BE->p_size = 1542 * byte;
            BE->p_priority = 0;
            BE->p_flow_id = 1;
            BE->source = 2;
            BE->destination = 1;
            //BE->send_time = time;
            BE->packet_id = packet_id[0]++;
            BE->deadline = 100000;
            ed[2]->sendPacket(BE);
            next_packet_send_time[0] += (100000 + rand()%2000 - 1000);
        }

        if(time == next_packet_send_time[1]) {
            Packet *AVB = new Packet();
            AVB->p_size = 400 * byte;
            AVB->p_priority = 5;
            AVB->p_flow_id = 2;
            AVB->source = 3;
            AVB->destination = 1;
            //AVB->send_time = time;
            AVB->packet_id = packet_id[1]++;
            AVB->deadline = 2000;
            ed[3]->sendPacket(AVB);
            next_packet_send_time[1] += (12500 + rand()%2000 - 1000);
        }

        if(time == next_packet_send_time[2]) {
            Packet *TSN1 = new Packet();
            TSN1->p_size = 128 * byte;
            TSN1->p_priority = 7;
            TSN1->p_flow_id = 3;
            TSN1->source = 4;
            TSN1->destination = 1;
            //TSN1->send_time = time;
            TSN1->packet_id = packet_id[2]++;
            TSN1->deadline = 130;
            ed[4]->sendPacket(TSN1);
            next_packet_send_time[2] += (15000 + rand()%2000 - 1000);
        }

        int rd = rand()%2000 - 1000;
        if(time == next_packet_send_time[3]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 4;
            TSN2->source = 5;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[3]++;
            TSN2->deadline = 200;
            ed[5]->sendPacket(TSN2);
            next_packet_send_time[3] += (20000 + rd);
        }

        if(time == next_packet_send_time[4]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 5;
            TSN2->source = 6;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[4]++;
            TSN2->deadline = 200;
            ed[6]->sendPacket(TSN2);
            next_packet_send_time[4] += (20000 + rd);
        }

        if(time == next_packet_send_time[5]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 6;
            TSN2->source = 7;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[5]++;
            TSN2->deadline = 200;
            ed[7]->sendPacket(TSN2);
            next_packet_send_time[5] += (20000 + rd);
        }

        if(time == next_packet_send_time[6]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 7;
            TSN2->source = 8;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[6]++;
            TSN2->deadline = 200;
            ed[8]->sendPacket(TSN2);
            next_packet_send_time[6] += (20000 + rd);
        }
/*
        if(time == next_packet_send_time[7]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 8;
            TSN2->source = 9;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[7]++;
            TSN2->deadline = 200;
            ed[9]->sendPacket(TSN2);
            next_packet_send_time[7] += (20000 + rd);
        }

        if(time == next_packet_send_time[8]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 9;
            TSN2->source = 10;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[8]++;
            TSN2->deadline = 200;
            ed[10]->sendPacket(TSN2);
            next_packet_send_time[8] += (20000 + rd);
        }

        if(time == next_packet_send_time[9]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 10;
            TSN2->source = 11;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[9]++;
            TSN2->deadline = 200;
            ed[11]->sendPacket(TSN2);
            next_packet_send_time[9] += (20000 + rd);
        }

        if(time == next_packet_send_time[10]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 11;
            TSN2->source = 12;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[10]++;
            TSN2->deadline = 200;
            ed[12]->sendPacket(TSN2);
            next_packet_send_time[10] += (20000 + rd);
        }

        if(time == next_packet_send_time[11]) {
            Packet *TSN2 = new Packet();
            TSN2->p_size = 64 * byte;
            TSN2->p_priority = 7;
            TSN2->p_flow_id = 12;
            TSN2->source = 13;
            TSN2->destination = 1;
            //TSN2->send_time = time;
            TSN2->packet_id = packet_id[11]++;
            TSN2->deadline = 200;
            ed[13]->sendPacket(TSN2);
            next_packet_send_time[11] += (20000 + rd);
        }
*/
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
