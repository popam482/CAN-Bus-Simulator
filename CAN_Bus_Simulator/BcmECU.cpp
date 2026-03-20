#include "BcmECU.h"
#include <iostream>
#include <chrono>

BcmECU::BcmECU(CANBus& b)
	: bus(b), headLightsOn(false), wipersOn(false), doorLockStatus(0), running(true) {

	doorState.frontLeftDoor = false;
	doorState.frontRightDoor = false;
	doorState.rearLeftDoor = false;
	doorState.rearRightDoor = false;

	lightState.headLights = false;
	lightState.tailLights = false;
	lightState.fogLights = false;

	bcmThread = std::thread(&BcmECU::bcmWorker, this);
}

BcmECU::~BcmECU() {
	shutdown();
}

std::string BcmECU::getName() {
	return "BCM_ECU";
}

void BcmECU::receiveFrame(CANFrame& frame) {
	{
		std::lock_guard<std::mutex> lock(frameMutex);
		frameQueue.push(frame);
	}
	cv.notify_one();
}

void BcmECU::bcmWorker() {
	while (running) {
		{
			std::unique_lock<std::mutex> lock(frameMutex);
			cv.wait_for(lock, std::chrono::milliseconds(500),
				[this] { return !frameQueue.empty() || !running; });

			if (!frameQueue.empty()) {
				CANFrame frame = frameQueue.front();
				frameQueue.pop();
				lock.unlock();

				if (frame.getId() == 0x101) {
					uint8_t speed = frame.getData(0);

					{
						std::lock_guard<std::mutex> lock(bcmMutex);
						if (isDarkOutside() && speed > 0 && !lightState.headLights) {
							std::cout << "  -> [BCM] Auto-enabling headlights (dark & moving)" << std::endl;
							lightState.headLights = true;
							lightState.tailLights = true;
						}
					}
				}

				if (frame.getId() == 0x201) {
					{
						std::lock_guard<std::mutex> lock(bcmMutex);
						std::cout << "  -> [BCM] Brake lights activated" << std::endl;
						lightState.tailLights = true;
					}
				}
			}
		}

		updateDoors();

		sendBcmStatus();

		std::this_thread::sleep_for(std::chrono::milliseconds(800));
	}
}

bool BcmECU::isDarkOutside() {
	static int cycleCounter = 0;
	cycleCounter++;
	return (cycleCounter / 3) % 2 == 0;
}


void BcmECU::updateDoors() {
	std::lock_guard<std::mutex> lock(bcmMutex);

	if (doorLockStatus == 15) {
		std::cout << "  -> [BCM] All doors LOCKED" << std::endl;
	}
	else if (doorLockStatus == 0) {
		std::cout << "  -> [BCM] All doors UNLOCKED" << std::endl;
	}
}

void BcmECU::sendBcmStatus() {
	CANFrame frame;
	frame.setId(0x801);  

	uint8_t status = 0;
	{
		std::lock_guard<std::mutex> lock(bcmMutex);
		if (lightState.headLights) status |= (1 << 6);
		if (lightState.tailLights) status |= (1 << 5);
		if (lightState.fogLights) status |= (1 << 4);

		status |= (doorLockStatus & 0x0F);
	}

	frame.setData({ status });
	bus.send(this, frame);
}

void BcmECU::lockDoors() {
	std::lock_guard<std::mutex> lock(bcmMutex);

	doorState.frontLeftDoor = true;
	doorState.frontRightDoor = true;
	doorState.rearLeftDoor = true;
	doorState.rearRightDoor = true;
	doorLockStatus = 15; 

	std::cout << "[BCM] Doors LOCKED" << std::endl;
}

void BcmECU::unlockDoors() {
	std::lock_guard<std::mutex> lock(bcmMutex);

	doorState.frontLeftDoor = false;
	doorState.frontRightDoor = false;
	doorState.rearLeftDoor = false;
	doorState.rearRightDoor = false;
	doorLockStatus = 0;

	std::cout << "[BCM] Doors UNLOCKED" << std::endl;
}

void BcmECU::turnHeadLights(bool on) {
	std::lock_guard<std::mutex> lock(bcmMutex);
	lightState.headLights = on;
	std::cout << "[BCM] Headlights " << (on ? "ON" : "OFF") << std::endl;
}

void BcmECU::turnTailLights(bool on) {
	std::lock_guard<std::mutex> lock(bcmMutex);
	lightState.tailLights = on;
	std::cout << "[BCM] Tail lights " << (on ? "ON" : "OFF") << std::endl;
}

void BcmECU::turnFogLights(bool on) {
	std::lock_guard<std::mutex> lock(bcmMutex);
	lightState.fogLights = on;
	std::cout << "[BCM] Fog lights " << (on ? "ON" : "OFF") << std::endl;
}

void BcmECU::isRaining(bool rain)
{
	std::lock_guard<std::mutex> lock(bcmMutex);
	wipersOn = rain;
	std::cout << "[BCM] Wipers " << (wipersOn ? "ON" : "OFF") << std::endl;

}

void BcmECU::displayStatus() {
	std::lock_guard<std::mutex> lock(bcmMutex);

	std::cout << "\n[BCM STATUS]" << std::endl;
	std::cout << "  Doors: " << (doorLockStatus == 15 ? "LOCKED" : "UNLOCKED") << std::endl;
	std::cout << "  Head Lights: " << (lightState.headLights ? "ON" : "OFF") << std::endl;
	std::cout << "  Tail Lights: " << (lightState.tailLights ? "ON" : "OFF") << std::endl;
	std::cout << "  Fog Lights: " << (lightState.fogLights ? "ON" : "OFF") << std::endl;
	std::cout << "  Wipers: " << (wipersOn ? "ON" : "OFF") << std::endl;
	std::cout << std::endl;
}

bool BcmECU::getHeadlightsStatus(){ return headLightsOn; }
bool BcmECU::getWiperStatus(){ return wipersOn; }

void BcmECU::setHeadlights(bool state) { headLightsOn = state; }
void BcmECU::setWipers(bool state) { wipersOn = state; }


void BcmECU::sendHeadlightsStatus() {
	CANFrame frame;
	frame.setId(0x601);
	uint8_t data = headLightsOn ? 1 : 0;
	frame.setData({ data });
	bus.send(this, frame);
}

void BcmECU::sendWipersStatus() {
	CANFrame frame;
	frame.setId(0x602);
	uint8_t data = wipersOn ? 1 : 0;
	frame.setData({ data });
	bus.send(this, frame);
}

void BcmECU::shutdown() {
	running = false;
	cv.notify_one();
	if (bcmThread.joinable()) {
		bcmThread.join();
	}
}