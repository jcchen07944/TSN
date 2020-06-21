#include <math.h>
#include <stdio.h>

#include "Port.h"
#include "Constants.h"

EDPort::EDPort(double rate) {
    this->rate = rate;

    _pforward = nullptr;
    _tforward = 0;
}

EDPort::~EDPort() {

}

void EDPort::run(long long time) {
    if(_pforward != nullptr) {
        if(time >= _tforward) {
            // Forwarding finish
            _pforward = nullptr;

            // Statistic
            printf("%d\n", time);
        }
    }
    if(_pforward == nullptr) {
        // Dequeue packet and put to forwarding state
        _pforward = buffer.front();

        sw->receivePacket(_pforward);

        _tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
        buffer.pop();
    }
}
