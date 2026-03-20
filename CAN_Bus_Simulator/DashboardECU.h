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

	uint8_t displayedSpeed;        
	uint16_t displayedRPM;
	uint8_t displayedFuelLevel;
	uint8_t displayedOilTemp;
	uint8_t displayedCoolantTemp;

	void displayWorker();

public:
	DashboardECU();
	~DashboardECU();

	std::string getName() override;
	void receiveFrame(CANFrame& frame) override;

	uint8_t getDisplayedSpeed();
	uint16_t getDisplayedRPM();
	uint8_t getDisplayedFuelLevel();
	uint8_t getDisplayedOilTemp();
	uint8_t getDisplayedCoolantTemp();

	void setDisplayedSpeed(uint8_t speed);
	void setDisplayedRPM(uint16_t rpm);
	void setDisplayedFuelLevel(uint8_t fuel);
	void setDisplayedOilTemp(uint8_t temp);
	void setDisplayedCoolantTemp(uint8_t temp);

	void shutdown();
};