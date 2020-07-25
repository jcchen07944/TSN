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


    int TSN_FLOW_COUNT = 16;
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

    utility.setupTSN(TSN[0], 450, 150, 6, 2, 0); // 0.71%
    utility.reserveTSN(TSN[0], sw, ed);

    utility.setupTSN(TSN[1], 300, 200, 6, 2, 10); // 1.87%
    utility.reserveTSN(TSN[1], sw, ed);

    utility.setupTSN(TSN[2], 150, 750, 6, 2, 15); // 4%
    utility.reserveTSN(TSN[2], sw, ed);

    utility.setupTSN(TSN[3], 300, 350, 6, 2, 15); // 0.93%
    utility.reserveTSN(TSN[3], sw, ed);

    utility.setupTSN(TSN[4], 150, 64, 6, 2, 0); // 0.34%
    utility.reserveTSN(TSN[4], sw, ed);

    utility.setupTSN(TSN[5], 450, 725, 6, 2, 30); // 1.29%
    utility.reserveTSN(TSN[5], sw, ed);

    utility.setupTSN(TSN[6], 150, 200, 6, 2, 0); // 1.07%
    utility.reserveTSN(TSN[6], sw, ed);

    utility.setupTSN(TSN[7], 300, 350, 6, 2, 15); // 0.93%
    utility.reserveTSN(TSN[7], sw, ed);

    utility.setupTSN(TSN[8], 450, 400, 4, 2, 0); // 0.71%
    utility.reserveTSN(TSN[8], sw, ed);

    utility.setupTSN(TSN[9], 300, 700, 4, 2, 0); // 1.87%
    utility.reserveTSN(TSN[9], sw, ed);

    utility.setupTSN(TSN[10], 150, 750, 4, 2, 15); // 4%
    utility.reserveTSN(TSN[10], sw, ed);

    utility.setupTSN(TSN[11], 300, 350, 4, 2, 15); // 0.93%
    utility.reserveTSN(TSN[11], sw, ed);

    utility.setupTSN(TSN[12], 150, 64, 4, 2, 0); // 0.34%
    utility.reserveTSN(TSN[12], sw, ed);

    utility.setupTSN(TSN[13], 450, 725, 4, 2, 30); // 1.29%
    utility.reserveTSN(TSN[13], sw, ed);

    utility.setupTSN(TSN[14], 150, 200, 4, 2, 0); // 1.07%
    utility.reserveTSN(TSN[14], sw, ed);

    utility.setupTSN(TSN[15], 300, 350, 4, 2, 15); // 0.93%
    utility.reserveTSN(TSN[15], sw, ed);

    srand(time(NULL));

    utility.resetNetworkTime(sw, ed);
    long long time = 0;
    while(time < 1000000) { // 1 second
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
    return 0;
}
