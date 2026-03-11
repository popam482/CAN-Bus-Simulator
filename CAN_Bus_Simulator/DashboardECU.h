#pragma once

#include <iostream>
#include "CANBus.h"

class DashboardECU: public ICanNode
{
public:
	std::string getName() override;

	void receiveFrame(CANFrame& frame) override;
};

