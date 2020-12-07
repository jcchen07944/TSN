#include <math.h>
#include <stdio.h>

#include "Port.h"
#include "Constants.h"

EDPort::EDPort(EndDevice *ed, double rate) {
    this->ed = ed;
    this->rate = rate;

    //_pforward = nullptr;
    //_tforward = 0;
}

EDPort::~EDPort() {

}

void EDPort::receivePacket(Packet* packet) {
    ed->receivePacket(packet);
}

void EDPort::run(long long time) {
    //if(_pforward != nullptr) {
    for(int i = _tforward.size() - 1; i >= 0; i--) {
        //if(time >= _tforward) {
        if(time >= _tforward[i]) {
            // Forwarding finish
            //sw_port->receivePacket(_pforward);
            sw_port->receivePacket(_pforward[i]);
            //_pforward = nullptr;
            _pforward.erase(_pforward.begin() + i);
            _tforward.erase(_tforward.begin() + i);
        }
    }
    //}
    //if(_pforward == nullptr) {
        while(buffer.size() != 0) {
            // Dequeue packet and put to forwarding state
            //_pforward = buffer.front();
            _pforward.push_back(buffer.front());
            //_tforward = time + (int)floor((double)_pforward->p_size / rate / us * 100.0d);
            _tforward.push_back(time + (int)floor((double)buffer.front()->p_size / rate / us * 100.0d));
            //_pforward->send_time = _tforward; // send time from first switch
            buffer.pop();
        }
    //}
}
