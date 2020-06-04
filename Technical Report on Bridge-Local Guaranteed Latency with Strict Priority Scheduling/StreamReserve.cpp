#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"

int main() {
    double delta3 = 2000.0;
    double delta2 = 8000.0;
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
    ed6->addNextHop(sw2);

    int priority[5] = {3, 3, 3, 2, 2};
    int burst[5] = {128, 256, 512, 1024, 1522};
    int interval[5] = {250, 500, 1000, 2000, 4000};
    int route[2][3] = {{3, 1, -1},
                        {0, 0, -1}};

    srand(time(NULL));
    int accept_count = 0;
    for(int i = 0; i < 500; ++i) {
        int random_flow_index = rand() % 5;
        int random_route_index = rand() % 2;
        Switch *first_switch = nullptr;

        Flow *flow = new Flow(i+1);
        flow->priority = priority[random_flow_index];
        flow->burst_size = burst[random_flow_index] * byte;
        flow->packet_length = burst[random_flow_index] * byte;
        flow->burst_interval = interval[random_flow_index] * us;
        if(random_route_index == 0) {
            route[random_route_index][2] = rand() % 3 + 1;
            first_switch = sw1;
        }
        else {
            route[random_route_index][2] = rand() % 3;
            first_switch = sw3;
        }
        flow->route = route[random_route_index];

        printf("Flow %d, p = %d, burst = %d, interval = %f\n", flow->ID, flow->priority, flow->burst_size, flow->burst_interval);
        if(first_switch->addFlow(flow)) {
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
