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


    int TSN_FLOW_COUNT = 0;
    int AVB_FLOW_COUNT = 1;
    std::vector<Flow*> TSN;
    std::vector<Flow*> AVB;
    for(int i = 0; i < TSN_FLOW_COUNT; i++)
        TSN.push_back(new Flow(i));
    for(int i = 0; i < AVB_FLOW_COUNT; i++)
        AVB.push_back(new Flow(i + TSN_FLOW_COUNT));

    utility.setupAVB(AVB[0], 'A', 3200, 3, 2, 0); // 12.8%
    //utility.setupAVB(AVB[1], 'A', 200, 6, 2, 0); // 12.8%

    //utility.setupTSN(TSN[0], 100, 200, 1, 2, 0); // 16%
    //utility.setupTSN(TSN[1], 200, 250, 4, 2, 0); // 10%
    //utility.setupTSN(TSN[2], 250, 450, 5, 2, 0); // 14.4%
    //utility.setupTSN(TSN[4], 100, 350, 1, 2, 0); // 28%
    //utility.setupTSN(TSN[5], 200, 300, 6, 2, 0);

    srand(time(NULL));

    long long time = 0;

    while(time < 100000000) { // 1 second
        for(int i = 0; i < TSN_FLOW_COUNT; i++)
            TSN[i]->run(ed[TSN[i]->source]);
        for(int i = 0; i < AVB_FLOW_COUNT; i++)
           AVB[i]->run(ed[AVB[i]->source]);

        for(int i = 0; i < END_DEVICE_COUNT; i++)
            ed[i]->run();
        for(int i = 0; i < SWITCH_COUNT; i++)
            sw[i]->run();

        time++;
    }

    return 0;
}
