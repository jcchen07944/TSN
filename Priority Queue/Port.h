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

    class TRComparison {
    public:
        bool operator() (Packet *a, Packet *b) {
            return (a->p_size > b->p_size);
        }
    };

    std::vector<std::queue<Packet*>*> t_queue;
    std::vector<std::priority_queue<Packet*, std::vector<Packet*>, Comparison>*> t_priority_queue;

    /* For Time-reservation */
    int current_slot;
    std::queue<Packet*> be_queue;
    int cycle; // time slot number in a cycle
    std::map<int, int> offset_table; // <flow_id, offset>
    std::map<int, int> time_slot; // <reserved_slot_number, packet_size(bit)>
    std::map<int, Packet*> reserved_table; // <flow_id, flow_info>
    std::map<int, int> queue_table; // <flow_id, queue_id>
    std::vector<int> gate_control_list; // <queue_id>
    std::vector<std::priority_queue<Packet*, std::vector<Packet*>, TRComparison> > scheduled_queue;
    double last_transmission_time;

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

    int findAcceptQueueID(Packet *packet, int offset);
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
