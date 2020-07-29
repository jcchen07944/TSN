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

    const int END_DEVICE_COUNT = 7;
    const int SWITCH_COUNT = 4;

    std::vector<EndDevice*> ed;
    std::vector<Switch*> sw;

    for(int i = 0; i < END_DEVICE_COUNT; i++)
        ed.push_back(new EndDevice(i));

    for(int i = 0; i < SWITCH_COUNT; i++)
        sw.push_back(new Switch(i + END_DEVICE_COUNT));

    utility.connectToSwitch(sw[0], ed[0]);
    utility.connectToSwitch(sw[0], ed[6]);

    utility.connectToSwitch(sw[1], ed[1]);

    utility.connectToSwitch(sw[2], ed[4]);
    utility.connectToSwitch(sw[2], ed[5]);

    utility.connectToSwitch(sw[3], ed[2]);
    utility.connectToSwitch(sw[3], ed[3]);

    utility.connectToSwitch(sw[0], sw[1]);
    utility.connectToSwitch(sw[0], sw[2]);
    utility.connectToSwitch(sw[1], sw[2]);
    utility.connectToSwitch(sw[1], sw[3]);

    utility.broadcastEndDevice(sw, ed);


    int TSN_FLOW_COUNT = 72;
    int AVB_FLOW_COUNT = 0;
    int BE_FLOW_COUNT = 9;
    std::vector<Flow*> TSN;
    std::vector<Flow*> AVB;
    std::vector<Flow*> BE;
    for(int i = 0; i < TSN_FLOW_COUNT; i++)
        TSN.push_back(new Flow(i));
    for(int i = 0; i < AVB_FLOW_COUNT; i++)
        AVB.push_back(new Flow(i + TSN_FLOW_COUNT));
    for(int i = 0; i < BE_FLOW_COUNT; i++)
        BE.push_back(new Flow(i + TSN_FLOW_COUNT + AVB_FLOW_COUNT));

    utility.setupBE(BE[0], 1500, 0, 2); // 120Mbps
    utility.setupBE(BE[1], 1500, 5, 2); // 120Mbps
    utility.setupBE(BE[2], 1500, 1, 2); // 120Mbps
    utility.setupBE(BE[3], 1500, 0, 2); // 120Mbps
    utility.setupBE(BE[4], 1500, 5, 2); // 120Mbps
    utility.setupBE(BE[5], 1500, 1, 2); // 120Mbps
    utility.setupBE(BE[6], 1500, 5, 2); // 120Mbps
    utility.setupBE(BE[7], 1500, 0, 2); // 120Mbps
    utility.setupBE(BE[8], 1500, 1, 2); // 120Mbps

    //utility.setupAVB(AVB[0], 'A', 400, 0, 1, 0); // 2.56%
    //utility.setupAVB(AVB[1], 'A', 800, 0, 1, 0); // 5.12%

    for(int i = 0; i < TSN_FLOW_COUNT / 24; i++) {
        utility.setupTSN(TSN[i*6], 450, 512, 4, 2, 0); //
        TSN[i*6]->hop_count = 3;
        TSN[i*6]->priority = 5;
        utility.reserveTSN(TSN[i*6], sw, ed);

        utility.setupTSN(TSN[i*6+1], 300, 512, 4, 2, 0); //
        TSN[i*6+1]->hop_count = 3;
        TSN[i*6+1]->priority = 6;
        utility.reserveTSN(TSN[i*6+1], sw, ed);

        utility.setupTSN(TSN[i*6+2], 150, 512, 4, 2, 0); //
        TSN[i*6+2]->hop_count = 3;
        TSN[i*6+2]->priority = 7;
        utility.reserveTSN(TSN[i*6+2], sw, ed);

        utility.setupTSN(TSN[i*6+3], 450, 512, 6, 2, 0); //
        TSN[i*6+3]->hop_count = 3;
        TSN[i*6+3]->priority = 5;
        utility.reserveTSN(TSN[i*6+3], sw, ed);

        utility.setupTSN(TSN[i*6+4], 300, 512, 6, 2, 0); //
        TSN[i*6+4]->hop_count = 3;
        TSN[i*6+4]->priority = 6;
        utility.reserveTSN(TSN[i*6+4], sw, ed);

        utility.setupTSN(TSN[i*6+5], 150, 512, 6, 2, 0); //
        TSN[i*6+5]->hop_count = 3;
        TSN[i*6+5]->priority = 7;
        utility.reserveTSN(TSN[i*6+5], sw, ed);
    }

    for(int i = TSN_FLOW_COUNT / 24; i < TSN_FLOW_COUNT / 24 * 2; i++) {
        utility.setupTSN(TSN[i*6], 450, 256, 4, 2, 0); //
        TSN[i*6]->hop_count = 3;
        TSN[i*6]->priority = 5;
        utility.reserveTSN(TSN[i*6], sw, ed);

        utility.setupTSN(TSN[i*6+1], 300, 256, 4, 2, 0); //
        TSN[i*6+1]->hop_count = 3;
        TSN[i*6+1]->priority = 6;
        utility.reserveTSN(TSN[i*6+1], sw, ed);

        utility.setupTSN(TSN[i*6+2], 150, 256, 4, 2, 0); //
        TSN[i*6+2]->hop_count = 3;
        TSN[i*6+2]->priority = 7;
        utility.reserveTSN(TSN[i*6+2], sw, ed);

        utility.setupTSN(TSN[i*6+3], 450, 256, 6, 2, 0); //
        TSN[i*6+3]->hop_count = 3;
        TSN[i*6+3]->priority = 5;
        utility.reserveTSN(TSN[i*6+3], sw, ed);

        utility.setupTSN(TSN[i*6+4], 300, 256, 6, 2, 0); //
        TSN[i*6+4]->hop_count = 3;
        TSN[i*6+4]->priority = 6;
        utility.reserveTSN(TSN[i*6+4], sw, ed);

        utility.setupTSN(TSN[i*6+5], 150, 256, 6, 2, 0); //
        TSN[i*6+5]->hop_count = 3;
        TSN[i*6+5]->priority = 7;
        utility.reserveTSN(TSN[i*6+5], sw, ed);
    }

    for(int i = TSN_FLOW_COUNT / 24 * 2; i < TSN_FLOW_COUNT / 24 * 3; i++) {
        utility.setupTSN(TSN[i*6], 450, 128, 4, 2, 0); //
        TSN[i*6]->hop_count = 3;
        TSN[i*6]->priority = 5;
        utility.reserveTSN(TSN[i*6], sw, ed);

        utility.setupTSN(TSN[i*6+1], 300, 128, 4, 2, 0); //
        TSN[i*6+1]->hop_count = 3;
        TSN[i*6+1]->priority = 6;
        utility.reserveTSN(TSN[i*6+1], sw, ed);

        utility.setupTSN(TSN[i*6+2], 150, 128, 4, 2, 0); //
        TSN[i*6+2]->hop_count = 3;
        TSN[i*6+2]->priority = 7;
        utility.reserveTSN(TSN[i*6+2], sw, ed);

        utility.setupTSN(TSN[i*6+3], 450, 128, 6, 2, 0); //
        TSN[i*6+3]->hop_count = 3;
        TSN[i*6+3]->priority = 5;
        utility.reserveTSN(TSN[i*6+3], sw, ed);

        utility.setupTSN(TSN[i*6+4], 300, 128, 6, 2, 0); //
        TSN[i*6+4]->hop_count = 3;
        TSN[i*6+4]->priority = 6;
        utility.reserveTSN(TSN[i*6+4], sw, ed);

        utility.setupTSN(TSN[i*6+5], 150, 128, 6, 2, 0); //
        TSN[i*6+5]->hop_count = 3;
        TSN[i*6+5]->priority = 7;
        utility.reserveTSN(TSN[i*6+5], sw, ed);
    }

    for(int i = TSN_FLOW_COUNT / 24 * 3; i < TSN_FLOW_COUNT / 24 * 4; i++) {
        utility.setupTSN(TSN[i*6], 450, 64, 4, 2, 0); //
        TSN[i*6]->hop_count = 3;
        TSN[i*6]->priority = 5;
        utility.reserveTSN(TSN[i*6], sw, ed);

        utility.setupTSN(TSN[i*6+1], 300, 64, 4, 2, 0); //
        TSN[i*6+1]->hop_count = 3;
        TSN[i*6+1]->priority = 6;
        utility.reserveTSN(TSN[i*6+1], sw, ed);

        utility.setupTSN(TSN[i*6+2], 150, 64, 4, 2, 0); //
        TSN[i*6+2]->hop_count = 3;
        TSN[i*6+2]->priority = 7;
        utility.reserveTSN(TSN[i*6+2], sw, ed);

        utility.setupTSN(TSN[i*6+3], 450, 64, 6, 2, 0); //
        TSN[i*6+3]->hop_count = 3;
        TSN[i*6+3]->priority = 5;
        utility.reserveTSN(TSN[i*6+3], sw, ed);

        utility.setupTSN(TSN[i*6+4], 300, 64, 6, 2, 0); //
        TSN[i*6+4]->hop_count = 3;
        TSN[i*6+4]->priority = 6;
        utility.reserveTSN(TSN[i*6+4], sw, ed);

        utility.setupTSN(TSN[i*6+5], 150, 64, 6, 2, 0); //
        TSN[i*6+5]->hop_count = 3;
        TSN[i*6+5]->priority = 7;
        utility.reserveTSN(TSN[i*6+5], sw, ed);
    }

    utility.resetNetworkTime(sw, ed);
    long long time = 0;
    while(time < 10000) { // 1 second
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

    printf("Maximum delay: %.4f, Average delay: %.4f\n", ed[2]->max_latency, ed[2]->acc_latency/ed[2]->flow_cnt);
    printf("Accept flow: %d, Reject flow: %d\n", ed[4]->accept_flow + ed[6]->accept_flow, ed[4]->reject_flow + ed[6]->reject_flow);
    return 0;
}
