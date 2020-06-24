#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"
#include "Packet.h"
#include "Utility.h"

int main() {
    Utility utility;

    const int END_DEVICE_COUNT = 5;
    const int SWITCH_COUNT = 2;

    std::vector<EndDevice*> ed;
    std::vector<Switch*> sw;

    for(int i = 0; i < END_DEVICE_COUNT; i++)
        ed.push_back(new EndDevice(i));

    for(int i = 0; i < SWITCH_COUNT; i++)
        sw.push_back(new Switch(i + END_DEVICE_COUNT));

    for(int i = 0; i < 4; i++)
        utility.connectToSwitch(sw[0], ed[i]);

    for(int i = 4; i < END_DEVICE_COUNT; i++)
        utility.connectToSwitch(sw[1], ed[i]);

    utility.connectToSwitch(sw[0], sw[1]);

    utility.broadcastEndDevice(sw, ed);


    int TSN_FLOW_COUNT = 2;
    int AVB_FLOW_COUNT = 6;
    std::vector<Flow*> TSN;
    std::vector<Flow*> AVB;
    for(int i = 0; i < TSN_FLOW_COUNT; i++) {
        TSN.push_back(new Flow(i));
        TSN[i]->burst_size = TSN[i]->packet_size = (rand() % 1499 + 1) * byte;
        int tmp = rand() % 3;
        TSN[i]->deadline = TSN[i]->period = (tmp == 0? 100*us: (tmp == 1? 200*us : 250*us);
        TSN[i]->source =
        TSN[i]->
    }
    for(int i = 0; i < AVB_FLOW_COUNT; i++)
        AVB.push_back(new Flow(i + TSN_FLOW_COUNT));



    srand(time(NULL));

    long long time = 0;

    long long next_packet_send_time[12] = {rand()%1000};
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

        for(int i = 0; i < END_DEVICE_COUNT; i++) {
            ed[i]->run();
        }

        for(int i = 0; i < SWITCH_COUNT; i++) {
            sw[i]->run();
        }
        time++;
    }

    return 0;
}
