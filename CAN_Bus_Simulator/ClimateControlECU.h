#pragma once

#include "CANBus.h"
#include <list>
#include <string>
#include <cmath>

class ClimateControlECU :public ICanNode
{
private:
	CANBus& bus;
	float targetTemp;
	float currentTemp;
	bool acOn;
	std::list<std::string> airDistribution; //"face", "windshield", "feet", "none"
	int ventSpeed; // 0-8

	std::queue<CANFrame> frameQueue;
	std::mutex tempMutex;
	std::mutex frameMutex;
	std::condition_variable cv;
	std::thread climateThread;
	std::atomic<bool> running;

	void climateWorker();
	void simulateTemperatureChange();
	void calculateAirDistribution();
	void sendClimateStatus();

public:
	ClimateControlECU(CANBus& b, float tTemp, float cTemp);
	~ClimateControlECU();

	std::string getName() override;
	void receiveFrame(CANFrame& frame) override;

	void turnAc();
	void displayStatus();
	void setTargetTemp(float temp);
	void shutdown();
	
};

