#pragma once

#include "CANBus.h"
#include <vector>
#include <iomanip>
#include <set>
#include <fstream>

enum class FaultCode {
	NO_FAULT = 0x00,
	OIL_TEMP_HIGH = 0x01,
	OIL_LEVEL_LOW = 0x02,
	COOLANT_TEMP_HIGH = 0x03,
	COOLANT_LEVEL_LOW = 0x04,
	FUEL_LEVEL_LOW = 0x05
};

class Diagnostics : public ICanNode {
private:
	CANBus& bus;

	float oilTemp;
	float oilLevel;
	float coolantTemp;
	float coolantLevel;
	float fuelLevel;

	std::set<FaultCode> activeFaults;

	std::queue<CANFrame> frameQueue;
	std::mutex diagMutex;
	std::mutex frameMutex;
	std::condition_variable cv;
	std::thread diagnosticThread;
	std::atomic<bool> running;

	void diagnosticWorker();
	void checkFaults();
	std::string getFaultDescription(FaultCode fault);

public:
	Diagnostics(CANBus& b);
	~Diagnostics();

	std::string getName() override;
	void receiveFrame(CANFrame& frame) override;
	void exportToFile(const std::string& filename);
	void displayFaults();
	void shutdown();
};