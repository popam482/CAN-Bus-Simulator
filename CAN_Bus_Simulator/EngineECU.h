#pragma once

#include <iostream>
#include "CANBus.h"

class EngineECU: public ICanNode
{
private:
	CANBus& bus;

public:
	EngineECU(CANBus& b) :bus(b) {};

	std::string getName() override;

	void sendSpeed(uint8_t speed);

	void receiveFrame(CANFrame &frame);

};

