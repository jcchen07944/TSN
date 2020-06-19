#include <math.h>
#include <stdio.h>

#include "Port.h"
#include "Constants.h"

Port::Port(double rate) {
    device = 0;
    sw = nullptr;
    ed = nullptr;
    this->rate = rate;

    for(int i = 0; i < 8; i++) {
        t_queue.push_back(new std::queue<Packet*>);
    }

    _pforward = nullptr;
    _tforward = 0;
}

Port::~Port() {
    delete sw;
    delete ed;
}

void Port::addDevice(Switch *sw) {
    this->sw = sw;
    device = SWITCH;
}

void Port::addDevice(EndDevice *ed) {
    this->ed = ed;
    device = END_DEVICE;
}

void Port::run(long long time) {
    if(_pforward != nullptr) {
        if(time >= _tforward) {
        printf("%d\n", time);
            // Forward packet to next hop
            if(device == SWITCH) {
                sw->recievePacket(_pforward);
            }
            else {
                ed->recievePacket(_pforward);
            }
            _pforward = nullptr;
        }
    }
    if(_pforward == nullptr) {
        // Strict Priority
        for(int i = t_queue.size() - 1; i >= 0; i--) {
            if(t_queue[i]->size() != 0) {
                // Dequeue packet and put to forwarding state
                _pforward = t_queue[i]->front();
                _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                t_queue[i]->pop();
                break;
            }
        }
    }
}
