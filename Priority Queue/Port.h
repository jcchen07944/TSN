#ifndef PORT_H
#define PORT_H

#include <vector>
#include <queue>

#include "Switch.h"
#include "EndDevice.h"
#include "Packet.h"

class SWPort;
class EDPort;
class Switch;
class EndDevice;

class Port {
public:
    int device;
    SWPort *sw_port;
    EDPort *ed_port;
    double rate;

    Port();

    ~Port();

    void addDevice(SWPort *sw_port);

    void addDevice(EDPort *ed_port);
protected:
    int SWITCH = 1;
    int END_DEVICE = 2;
};

class SWPort : public Port {
public:
    class Comparison {
    public:
        bool operator() (Packet *a, Packet *b) {
            if(a->deadline == b->deadline)
                return (a->sequence_number > b->sequence_number);
            return (a->deadline > b->deadline);
        }
    };

    std::vector<std::queue<Packet*>*> t_queue;
    std::vector<std::priority_queue<Packet*, std::vector<Packet*>, Comparison>*> t_priority_queue;

    /* For Time-reservation */
    std::vector<std::queue<Packet*> > offset_slot; // Reduce number of slots during switch running
    int current_slot;
    std::queue<Packet*> be_queue;
    int cycle; // time slot number in a cycle
    std::map<int, int> offset_table; // <flow_id, offset>
    std::map<int, int> time_slot; // <reserved_slot_number, packet_size(bit)>
    std::map<int, Packet*> reserved_table; // <flow_id, flow_info>


    int port_num;
    Switch *sw;

    void receivePacket(Packet* packet);

    SWPort(int port_num, Switch *sw, double rate);

    ~SWPort();

    void run(long long time);
private:
    Packet *_pforward; // Packet forwarding
    long long _tforward; // Forwarding time

    bool reserveTimeSlot(Packet *packet);

    void acceptTimeSlot(Packet *packet);
};

class EDPort : public Port {
public:
    std::queue<Packet*> buffer;

    EndDevice *ed;

    EDPort(EndDevice *ed, double rate);

    ~EDPort();

    void receivePacket(Packet* packet);

    void run(long long time);
private:
    Packet *_pforward; // Packet forwarding
    long long _tforward; // Forwarding time
};

#endif
