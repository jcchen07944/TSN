#include <algorithm>

#include "ATSScheduler.h"
#include "Constants.h"

ATSScheduler::ATSScheduler() {

}

ATSScheduler::ATSScheduler(int ID, int group_id) {
    this->ID = ID;
    this->group_id = group_id;
}

ATSScheduler::~ATSScheduler() {

}

long long ATSScheduler::getEligibilityTime(Packet *packet, std::vector<long long> group_eligibility_time) {
    int length_recovery_duration = (int)ceil((double)packet->p_size / (committed_information_rate / us / 100));
    int empty_to_full_duration = (int)ceil((double)committed_burst_size / (committed_information_rate / us / 100));
    long long scheduler_eligibility_time = bucket_empty_time + length_recovery_duration;
    long long bucket_full_time = bucket_empty_time + empty_to_full_duration;
    long long eligibility_time = std::max(packet->arrival_time, std::max(group_eligibility_time[group_id], scheduler_eligibility_time));
    //printf("%lld\n", eligibility_time);
    group_eligibility_time[group_id] = eligibility_time;
    bucket_empty_time = (eligibility_time < bucket_full_time)? scheduler_eligibility_time : scheduler_eligibility_time + eligibility_time - bucket_full_time;

    return eligibility_time;
}
