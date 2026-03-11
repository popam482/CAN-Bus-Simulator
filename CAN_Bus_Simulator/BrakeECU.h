#pragma once

#include "CANBus.h"

class BrakeECU:public ICanNode
{
private:
	CANBus& bus;

public:
	BrakeECU(CANBus& b) :bus(b) {};

	std::string getName() override;

	void pressBrake();

	void receiveFrame(CANFrame& frame) override;

};

