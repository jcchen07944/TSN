#include <stdio.h>

#include "Switch.h"
#include "EndDevice.h"
#include "Flow.h"
#include "Constants.h"

int main() {
    Switch *sw1 = new Switch(4);
    sw1->guarantee_delay[0] = 0;
    sw1->guarantee_delay[1] = 0;
    sw1->guarantee_delay[2] = 250 * us;
    sw1->guarantee_delay[3] = 100 * us;
    sw1->guarantee_delay[4] = 0;
    sw1->guarantee_delay[5] = 0;
    sw1->guarantee_delay[6] = 0;
    sw1->guarantee_delay[7] = 0;

    Switch *sw2 = new Switch(2);
    sw2->guarantee_delay[0] = 0;
    sw2->guarantee_delay[1] = 0;
    sw2->guarantee_delay[2] = 250 * us;
    sw2->guarantee_delay[3] = 100 * us;
    sw2->guarantee_delay[4] = 0;
    sw2->guarantee_delay[5] = 0;
    sw2->guarantee_delay[6] = 0;
    sw2->guarantee_delay[7] = 0;

    Switch *sw3 = new Switch(4);
    sw3->guarantee_delay[0] = 0;
    sw3->guarantee_delay[1] = 0;
    sw3->guarantee_delay[2] = 250 * us;
    sw3->guarantee_delay[3] = 100 * us;
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

    Flow *flow = new Flow(1);
    flow->priority = 3;
    flow->burst_size = 128 * byte;
    flow->packet_length = 128 * byte;
    flow->burst_interval = 250 * us;
    flow->max_hop = 3;
    flow->route = new int[3]{3, 1, 1};
    flow->nextHop();

    sw1->addFlow(flow);

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
