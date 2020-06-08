#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"

int main() {
    /*double delta3 = 500.0;
    double delta2 = 2000.0;
    Switch *sw1 = new Switch(1, 4);
    sw1->guarantee_delay[0] = 0;
    sw1->guarantee_delay[1] = 0;
    sw1->guarantee_delay[2] = delta2 * us;
    sw1->guarantee_delay[3] = delta3 * us;
    sw1->guarantee_delay[4] = 0;
    sw1->guarantee_delay[5] = 0;
    sw1->guarantee_delay[6] = 0;
    sw1->guarantee_delay[7] = 0;

    Switch *sw2 = new Switch(2, 2);
    sw2->guarantee_delay[0] = 0;
    sw2->guarantee_delay[1] = 0;
    sw2->guarantee_delay[2] = delta2 * us;
    sw2->guarantee_delay[3] = delta3 * us;
    sw2->guarantee_delay[4] = 0;
    sw2->guarantee_delay[5] = 0;
    sw2->guarantee_delay[6] = 0;
    sw2->guarantee_delay[7] = 0;

    Switch *sw3 = new Switch(3, 4);
    sw3->guarantee_delay[0] = 0;
    sw3->guarantee_delay[1] = 0;
    sw3->guarantee_delay[2] = delta2 * us;
    sw3->guarantee_delay[3] = delta3 * us;
    sw3->guarantee_delay[4] = 0;
    sw3->guarantee_delay[5] = 0;
    sw3->guarantee_delay[6] = 0;
    sw3->guarantee_delay[7] = 0;

    EndDevice *ed1 = new EndDevice();
    EndDevice *ed2 = new EndDevice();
    EndDevice *ed3 = new EndDevice();
    EndDevice *ed4 = new EndDevice();
    EndDevice *ed5 = new EndDevice();
    EndDevice *ed6 = new EndDevice();

    sw2->addNextHop(0, sw1, nullptr);
    sw2->addNextHop(1, sw3, nullptr);
    sw1->addNextHop(0, nullptr, ed1);
    sw1->addNextHop(1, nullptr, ed2);
    sw1->addNextHop(2, nullptr, ed3);
    sw1->addNextHop(3, sw2, nullptr);
    sw3->addNextHop(0, sw2, nullptr);
    sw3->addNextHop(1, nullptr, ed4);
    sw3->addNextHop(2, nullptr, ed5);
    sw3->addNextHop(3, nullptr, ed6);
    ed1->addNextHop(sw1);
    ed2->addNextHop(sw1);
    ed3->addNextHop(sw1);
    ed4->addNextHop(sw2);
    ed5->addNextHop(sw2);
    ed6->addNextHop(sw2);*/

    Switch *sw1 = new Switch(1, 2);
    sw1->guarantee_delay[0] = 100 * ms;
    sw1->guarantee_delay[1] = 1 * ms;
    sw1->guarantee_delay[2] = 250 * us;
    sw1->guarantee_delay[3] = 0;
    sw1->guarantee_delay[4] = 0;
    sw1->guarantee_delay[5] = 0;
    sw1->guarantee_delay[6] = 0;
    sw1->guarantee_delay[7] = 0;

    EndDevice *ed1 = new EndDevice();
    EndDevice *ed2 = new EndDevice();

    sw1->addNextHop(0, nullptr, ed1);
    sw1->addNextHop(1, nullptr, ed2);
    ed1->addNextHop(sw1);
    ed2->addNextHop(sw1);

    int priority[3] = {0, 1, 2};
    int burst[3] = {1500, 256, 64};
    int interval[3] = {100000, 1000, 250};
    int route[1][1] = {{1}};

    Flow *flow = new Flow(1);
    flow->priority = priority[0];
    flow->burst_size = burst[0] * byte;
    flow->packet_length = burst[0] * byte;
    flow->burst_interval = (double)interval[0] * us;
    flow->route = route[0];
    sw1->addFlow(flow);

    for(int i = 0; i < 20; ++i) {
        Flow *flow = new Flow(i+2);
        flow->priority = priority[1];
        flow->burst_size = burst[1] * byte;
        flow->packet_length = burst[1] * byte;
        flow->burst_interval = (double)interval[1] * us;
        flow->route = route[0];
        sw1->addFlow(flow);
    }

    int accept_count = 0;
    for(int i = 0; i < 371; ++i) {
        Flow *flow = new Flow(i+22);
        flow->priority = priority[2];
        flow->burst_size = burst[2] * byte;
        flow->packet_length = burst[2] * byte;
        flow->burst_interval = (double)interval[2] * us;
        flow->route = route[0];

        printf("Flow %d, p = %d, burst = %d, interval = %f\n", flow->ID, flow->priority, flow->burst_size, flow->burst_interval);
        if(sw1->addFlow(flow)) {
            accept_count++;
            printf("Accept\n");
        }
        else {
            printf("Reject\n");
        }
        printf("--------------------\n");
    }
    printf("Accept flows count : %d\n", accept_count);


    /*delete ed6;
    delete ed5;
    delete ed4;
    delete ed3;
    delete ed2;
    delete ed1;
    delete sw3;
    delete sw2;
    delete sw1;
    delete flow;*/
    return 0;
}
