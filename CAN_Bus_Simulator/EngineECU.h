#pragma once

#include <iostream>
#include "CANBus.h"

struct FluidData {
	float temperature;
	float level; //0.0f-1.0f
};

class EngineECU: public ICanNode
{
private:
	CANBus& bus;

	std::queue<uint8_t> messageQueue;
	std::mutex queueMutex;
	std::condition_variable cv;
	std::thread senderThread;
	std::atomic<bool> running;

	uint8_t currentSpeed;

	FluidData oil;
	FluidData coolant;

	float fuelTankLevel;  // 0.0f-1.0f (0-100%)
	float fuelConsumption; 
	uint16_t engineRPM;

	void senderWorker();
	void updateFluidLevels(uint8_t speed);

	void updateFuelLevel(uint8_t speed);

public:
	EngineECU(CANBus& b);
	~EngineECU();

	std::string getName() override;
	void sendSpeed(uint8_t speed);
	void receiveFrame(CANFrame& frame) override;

	void calculateRPM(uint8_t speed);
	void calculateFuelConsumption(uint8_t speed);

	void getOilData();
	void getCoolantData();
	void getFuelData();
	void getRPMData();

	void shutdown();

	void setOilTemp(float temp);
	void setOilLevel(float level);
	void setCoolantTemp(float temp);
	void setCoolantLevel(float level);
	void setFuelLevel(float level);

	float getOilTemp();
	float getOilLevel();
	float getCoolantTemp();
	float getCoolantLevel();
	float getFuelLevel();
	float getFuelConsumption();
	uint16_t getEngineRPM();
	uint8_t getCurrentSpeed();

};
