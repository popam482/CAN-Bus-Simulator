#pragma once

#include <iostream>
#include "CANBus.h"

class EngineECU: public ICanNode
{
private:
	CANBus& bus;
	std::queue<uint8_t> messageQueue;
	std::mutex queueMutex;
	std::condition_variable cv;
	std::thread senderThread;
	std::atomic<bool> running;

	void senderWorker();

public:
	EngineECU(CANBus& b);
	~EngineECU();

	std::string getName() override;
	void sendSpeed(uint8_t speed);
	void receiveFrame(CANFrame& frame) override;
	void shutdown();

};

