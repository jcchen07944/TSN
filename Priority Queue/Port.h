#ifndef PORT_H
#define PORT_H

#include <vector>
#include <queue>

#include "Switch.h"
#include "EndDevice.h"
#include "Packet.h"
#include "ATSScheduler.h"

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

    std::queue<Packet*> be_queue;
    std::vector<std::pair<Packet*, long long> > scheduled_buffer; // <packet, scheduled_slot> or <packet, scheduled_time>
    std::map<int, Packet*> reserved_table; // <flow_id, flow_info>

    int buffer_used;

    /* For Time-reservation */
    int current_slot;
    int cycle; // time slot number in a cycle
    std::map<int, int> offset_table; // <flow_id, offset>
    std::map<int, int> time_slot; // <reserved_slot_number, packet_size(bit)>
    double last_transmission_time;

    /* For ATS */
    std::map<int, int> ats_scheduler_table; // <flow_id, scheduler_id>
    std::vector<ATSScheduler*> ats_scheduler_list;
    std::vector<long long> group_eligibility_time;

    int port_num;
    Switch *sw;

    void receivePacket(Packet* packet);

    void sendPacket(Packet* packet);

    SWPort(int port_num, Switch *sw, double rate);

    ~SWPort();

    void run(long long time);
private:
    Packet *_pforward; // Packet forwarding
    long long _tforward; // Forwarding time

    bool reserveTimeSlot(Packet *packet);

    void acceptTimeSlot(Packet *packet);

    bool reserveBandwidth(Packet *packet);

    void acceptBandwidth(Packet *packet);

    int getEligibilitySlot(Packet* packet);
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
    std::vector<Packet*> _pforward;
    std::vector<long long> _tforward;
};

#endif
