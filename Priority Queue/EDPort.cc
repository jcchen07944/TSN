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
        }
    }
    if(_pforward == nullptr) {

    }
}
