#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <ctime>
#include <random>
#include <cstdlib>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"
#include "Packet.h"
#include "Utility.h"

int main() {
    Utility utility;

    const int END_DEVICE_COUNT = 8;
    const int SWITCH_COUNT = 4;

    std::vector<EndDevice*> ed;
    std::vector<Switch*> sw;

    for(int i = 0; i < END_DEVICE_COUNT; i++)
        ed.push_back(new EndDevice(i));

    for(int i = 0; i < SWITCH_COUNT; i++)
        sw.push_back(new Switch(i + END_DEVICE_COUNT));

    // Ring Topology
    utility.connectToSwitch(sw[0], ed[0]);
    utility.connectToSwitch(sw[0], ed[1]);
    utility.connectToSwitch(sw[1], ed[2]);
    utility.connectToSwitch(sw[1], ed[3]);
    utility.connectToSwitch(sw[2], ed[4]);
    utility.connectToSwitch(sw[2], ed[5]);
    utility.connectToSwitch(sw[3], ed[6]);
    utility.connectToSwitch(sw[3], ed[7]);

    utility.connectToSwitch(sw[0], sw[1]);
    utility.connectToSwitch(sw[1], sw[2]);
    utility.connectToSwitch(sw[2], sw[3]);
    utility.connectToSwitch(sw[3], sw[0]);

    utility.broadcastEndDevice(sw, ed);

    int TSN_FLOW_COUNT = 2000;
    int AVB_FLOW_COUNT = 0;
    int BE_FLOW_COUNT = 0;
    std::vector<Flow*> TSN;
    std::vector<Flow*> AVB;
    std::vector<Flow*> BE;
    for(int i = 0; i < TSN_FLOW_COUNT; i++)
        TSN.push_back(new Flow(i + 1));
    for(int i = 0; i < AVB_FLOW_COUNT; i++)
        AVB.push_back(new Flow(i + TSN_FLOW_COUNT + 1));
    for(int i = 0; i < BE_FLOW_COUNT; i++)
        BE.push_back(new Flow(i + TSN_FLOW_COUNT + AVB_FLOW_COUNT + 1));
    for(int i = 0; i < BE_FLOW_COUNT; i++) {
        utility.setupBE(BE[i], 1500, 0, 1); // 120Mbps
    }

    //utility.setupAVB(AVB[0], 'A', 400, 0, 1, 0);
    //utility.setupAVB(AVB[1], 'A', 800, 0, 1, 0);

    //FILE *pFile;
    //pFile = fopen("Computation.txt", "w");

    std::default_random_engine generator;

    for(int i = 0; i < TSN_FLOW_COUNT; i++) {
        std::normal_distribution<double> period_distribution(3, 1);
        std::normal_distribution<double> size_distribution(65, 10);
        int period = (int)period_distribution(generator) * 150 + 100;
        int packet_size = (int)size_distribution(generator);
        std::poisson_distribution<int> time_distribution(period / 2);
        int start_transmission_time = time_distribution(generator);
        std::uniform_int_distribution<int> src_distribution(0, 3);
        std::uniform_int_distribution<int> dst_distribution(0, 3);
        int src = src_distribution(generator) * 2;
        int dst = dst_distribution(generator) * 2 + 1;
        int hop_count = std::min(std::abs(src/2 - dst/2), 4 - std::abs(src/2 - dst/2)) + 1;
        printf("Period : %d, Packet Size : %d, Time : %d, Src : %d, Dst : %d, Hop Count : %d\n", period, packet_size, start_transmission_time, src, dst, hop_count);
        utility.setupTSN(TSN[i], period, packet_size, src, dst, 0);
        TSN[i]->hop_count = hop_count;
        TSN[i]->priority = 7 - (period - 100)/150;
        // Timer
        std::clock_t timer_start = std::clock();

        utility.reserveTSN(TSN[i], sw, ed);

        double duration = (std::clock() - timer_start) / (double) CLOCKS_PER_SEC;
        //fprintf (pFile, "%d %f\n", i+1, duration);
    }
    //return 0;

    //fclose (pFile);

    utility.resetNetworkTime(sw, ed);
    long long time = 0;
    while(time > 100000) { // 1 second
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

    //printf("Maximum delay: %.4f, Average delay: %.4f\n", ed[1]->max_latency, ed[1]->acc_latency/ed[1]->flow_cnt);
    int accept_flow = 0, reject_flow = 0;
    for(int i = 0; i < END_DEVICE_COUNT; i++) {
        accept_flow += ed[i]->accept_flow;
        reject_flow += ed[i]->reject_flow;
    }

    printf("Accept flow: %d, Reject flow: %d\n", accept_flow, reject_flow);
    //printf("Max buffer used: \n  Switch1: %d\n  Switch2: %d\n", sw[0]->max_buffer_used, sw[1]->max_buffer_used);
    return 0;
}
