#pragma once

#include "CANBus.h"
#include <iostream>

class TransmissionECU:public ICanNode
{
private:
	CANBus& bus;
	uint8_t currentGear;

public:
	TransmissionECU(CANBus& b) :bus(b), currentGear(1) {}

	std::string getName() override;

	void receiveFrame(CANFrame& frame) override;

	void sendCurrentGear();
};

