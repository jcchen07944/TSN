#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <ctime>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"
#include "Packet.h"
#include "Utility.h"

int main() {
    Utility utility;

    const int END_DEVICE_COUNT = 2;
    const int SWITCH_COUNT = 1;

    std::vector<EndDevice*> ed;
    std::vector<Switch*> sw;

    for(int i = 0; i < END_DEVICE_COUNT; i++)
        ed.push_back(new EndDevice(i));

    for(int i = 0; i < SWITCH_COUNT; i++)
        sw.push_back(new Switch(i + END_DEVICE_COUNT));

    utility.connectToSwitch(sw[0], ed[0]);
    utility.connectToSwitch(sw[0], ed[1]);
    //utility.connectToSwitch(sw[1], sw[2]);

    utility.broadcastEndDevice(sw, ed);

    int TSN_FLOW_COUNT = 200;
    int AVB_FLOW_COUNT = 0;
    int BE_FLOW_COUNT = 0;
    std::vector<Flow*> TSN;
    std::vector<Flow*> AVB;
    std::vector<Flow*> BE;
    for(int i = 0; i < TSN_FLOW_COUNT; i++)
        TSN.push_back(new Flow(i));
    for(int i = 0; i < AVB_FLOW_COUNT; i++)
        AVB.push_back(new Flow(i + TSN_FLOW_COUNT));
    for(int i = 0; i < BE_FLOW_COUNT; i++)
        BE.push_back(new Flow(i + TSN_FLOW_COUNT + AVB_FLOW_COUNT));
    for(int i = 0; i < BE_FLOW_COUNT; i++) {
        utility.setupBE(BE[i], 1500, 0, 1); // 120Mbps
    }

    //utility.setupAVB(AVB[0], 'A', 400, 0, 1, 0); // 2.56%
    //utility.setupAVB(AVB[1], 'A', 800, 0, 1, 0); // 5.12%

    FILE *pFile;
    pFile = fopen("Computation.txt", "w");

    for(int i = 0; i < TSN_FLOW_COUNT; i++) {
        utility.setupTSN(TSN[i], 150, 64, 0, 1, 0);
        TSN[i]->hop_count = 1;
        TSN[i]->priority = 7;
        // Timer
        std::clock_t timer_start = std::clock();

        utility.reserveTSN(TSN[i], sw, ed);

        double duration = (std::clock() - timer_start) / (double) CLOCKS_PER_SEC;
        fprintf (pFile, "%d %f\n", i+1, duration);
    }

    fclose (pFile);

    utility.resetNetworkTime(sw, ed);
    long long time = 0;
    while(time < 100000) { // 1 second
        for(int i = 0; i < TSN_FLOW_COUNT; i++)
            TSN[i]->run(ed[TSN[i]->source], TSN_FLOW);
        for(int i = 0; i < AVB_FLOW_COUNT; i++)
            AVB[i]->run(ed[AVB[i]->source], AVB_FLOW);
        for(int i = 0; i < BE_FLOW_COUNT; i++)
            BE[i]->run(ed[BE[i]->source], BE_FLOW);

        for(int i = 0; i < END_DEVICE_COUNT; i++)
            ed[i]->run();
        for(int i = 0; i < SWITCH_COUNT; i++)
            sw[i]->run();

        time++;
    }

    printf("Maximum delay: %.4f, Average delay: %.4f\n", ed[1]->max_latency, ed[1]->acc_latency/ed[1]->flow_cnt);
    printf("Accept flow: %d, Reject flow: %d\n", ed[0]->accept_flow, ed[0]->reject_flow);
    printf("Max buffer used: \n  Switch1: %d\n  Switch2: %d\n", sw[0]->max_buffer_used, sw[1]->max_buffer_used);
    return 0;
}
