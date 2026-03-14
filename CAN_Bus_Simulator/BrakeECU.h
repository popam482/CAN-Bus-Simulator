#pragma once

#include "CANBus.h"
#include <thread>
#include <mutex>
#include <atomic>

class BrakeECU : public ICanNode
{
private:
	CANBus& bus;
	std::atomic<bool> brakePressured;
	std::thread brakeThread;
	std::atomic<bool> running;

	void brakeWorker();

public:
	BrakeECU(CANBus& b);
	~BrakeECU();

	std::string getName() override;
	void pressBrake();
	void receiveFrame(CANFrame& frame) override;
	void shutdown();
};