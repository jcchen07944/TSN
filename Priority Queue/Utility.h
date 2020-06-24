#ifndef UTILITY_H
#define UTILITY_H

#include "Switch.h"
#include "EndDevice.h"

class Utility {
public:
	Utility();

	~Utility();

    void connectToSwitch(Switch *sw1, Switch *sw2);

    void connectToSwitch(Switch *sw, EndDevice *ed);

	void broadcastEndDevice(std::vector<Switch*> sw, std::vector<EndDevice*> ed);
};

#endif
