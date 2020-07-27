#ifndef UTILITY_H
#define UTILITY_H

#include "Switch.h"
#include "EndDevice.h"

class Utility {
public:
    int hop_count; // For ATS reservation

	Utility();

	~Utility();

    void connectToSwitch(Switch *sw1, Switch *sw2);

    void connectToSwitch(Switch *sw, EndDevice *ed);

	void broadcastEndDevice(std::vector<Switch*> sw, std::vector<EndDevice*> ed);

	void setupTSN(Flow *TSN, double period, int packet_size, int source, int destination, int start_time);

	void setupTSN(Flow *TSN, double period, double deadline, int packet_size, int source, int destination, int start_time);

	void reserveTSN(Flow *TSN, std::vector<Switch*> sw, std::vector<EndDevice*> ed);

	void setupAVB(Flow *AVB, char SRClass, int packet_size, int source, int destination, int start_time);

	void setupBE(Flow *BE, int packet_size, int source, int destination);

	int gcd(int m, int n);

	int lcm(int m, int n);

	void resetNetworkTime(std::vector<Switch*> sw, std::vector<EndDevice*> ed);
};

#endif
