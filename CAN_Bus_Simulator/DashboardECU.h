#pragma once

#include <iostream>
#include "CANBus.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class DashboardECU : public ICanNode
{
private:
	std::queue<CANFrame> displayQueue;
	std::mutex displayMutex;
	std::condition_variable cv;
	std::thread displayThread;
	std::atomic<bool> running;

	void displayWorker();

public:
	DashboardECU();
	~DashboardECU();

	std::string getName() override;
	void receiveFrame(CANFrame& frame) override;
	void shutdown();
};