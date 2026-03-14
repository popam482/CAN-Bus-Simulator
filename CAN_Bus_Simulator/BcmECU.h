#pragma once

#include "CANBus.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>

struct DoorState {
	bool frontLeftDoor;
	bool frontRightDoor;
	bool rearLeftDoor;
	bool rearRightDoor;
	bool trunkDoor;
};

struct LightState {
	bool headLights;
	bool tailLights;
	bool fogLights;
};

class BcmECU : public ICanNode
{
private:
	CANBus& bus;
	DoorState doorState;
	LightState lightState;
	bool wipersOn;
	int doorLockStatus;  

	std::queue<CANFrame> frameQueue;
	std::mutex bcmMutex;
	std::mutex frameMutex;
	std::condition_variable cv;
	std::thread bcmThread;
	std::atomic<bool> running;

	void bcmWorker();
	void updateDoors();
	void sendBcmStatus();
	bool isDarkOutside();

public:
	BcmECU(CANBus& b);
	~BcmECU();

	std::string getName() override;
	void receiveFrame(CANFrame& frame) override;

	void lockDoors();
	void unlockDoors();
	void turnHeadLights(bool on);
	void turnTailLights(bool on);
	void turnFogLights(bool on);
	void isRaining(bool rain);
	void displayStatus();
	void shutdown();
};