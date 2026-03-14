#pragma once

#include "CANBus.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class TransmissionECU : public ICanNode
{
private:
	CANBus& bus;
	uint8_t currentGear;
	std::queue<CANFrame> frameQueue;
	std::mutex frameMutex;
	std::condition_variable cv;
	std::thread processorThread;
	std::atomic<bool> running;

	void processorWorker();

public:
	TransmissionECU(CANBus& b);
	~TransmissionECU();

	std::string getName() override;
	void receiveFrame(CANFrame& frame) override;
	void sendCurrentGear();
	void shutdown();
};