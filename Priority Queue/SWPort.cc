#include <math.h>
#include <stdio.h>

#include "Port.h"
#include "Constants.h"

SWPort::SWPort(double rate) {
    this->rate = rate;

    for(int i = 0; i < 8; i++) {
        t_queue.push_back(new std::queue<Packet*>);
        t_priority_queue.push_back(new std::priority_queue<Packet*, std::vector<Packet*>, Comparison>);
    }

    _pforward = nullptr;
    _tforward = 0;
}

SWPort::~SWPort() {

}

void SWPort::run(long long time) {
    if(_pforward != nullptr) {
        if(time >= _tforward) {
            // Forwarding finish
            _pforward = nullptr;
        }
    }
    if(_pforward == nullptr) {
        // Strict Priority
        // FIFO
        if(!priority_queue_enable) {
            for(int i = t_queue.size() - 1; i >= 0; i--) {
                if(t_queue[i]->size() != 0) {
                    // Dequeue packet and put to forwarding state
                    _pforward = t_queue[i]->front();
                    if(device == SWITCH) {
                        sw->receivePacket(_pforward);
                    }
                    else {
                        ed->receivePacket(_pforward);
                    }
                    _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                    t_queue[i]->pop();
                    break;
                }
            }
        }
        // Priority Queue
        else {
            for(int i = t_priority_queue.size() - 1; i >= 0; i--) {
                if(t_priority_queue[i]->size() != 0) {
                    _pforward = t_priority_queue[i]->top();
                    if(device == SWITCH) {
                        sw->receivePacket(_pforward);
                    }
                    else {
                        ed->receivePacket(_pforward);
                    }
                    _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
                    t_priority_queue[i]->pop();
                    break;
                }
            }
        }
    }
}
