#ifndef ATSSCHEDULER_H
#define ATSSCHEDULER_H

#include "Packet.h"

class ATSScheduler {
public:
    int ID;

    int group_id;

    double committed_information_rate; // Bits / Second

    int committed_burst_size;

    long long bucket_empty_time;

    ATSScheduler();

    ATSScheduler(int ID, int group_id);

    ~ATSScheduler();

    long long getEligibilityTime(Packet *packet, std::vector<long long> group_eligibility_time);
private:
};



#endif
