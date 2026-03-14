#include "ClimateControlECU.h"
#include <iostream>
#include <chrono>

ClimateControlECU::ClimateControlECU(CANBus& b, float tTemp, float cTemp)
	: bus(b), targetTemp(tTemp), currentTemp(cTemp), acOn(false), ventSpeed(0), running(true) {

	climateThread = std::thread(&ClimateControlECU::climateWorker, this);
}

ClimateControlECU::~ClimateControlECU() {
	shutdown();
}

std::string ClimateControlECU::getName() {
	return "Climate_Control_ECU";
}

void ClimateControlECU::receiveFrame(CANFrame& frame) {
	{
		std::lock_guard<std::mutex> lock(frameMutex);
		frameQueue.push(frame);
	}
	cv.notify_one();
}

void ClimateControlECU::climateWorker() {
	while (running) {
		{
			std::unique_lock<std::mutex> lock(frameMutex);
			cv.wait_for(lock, std::chrono::milliseconds(500),
				[this] { return !frameQueue.empty() || !running; });

			if (!frameQueue.empty()) {
				CANFrame frame = frameQueue.front();
				frameQueue.pop();
				lock.unlock();

				if (frame.getId() == 0x601) {  
					{
						std::lock_guard<std::mutex> tempLock(tempMutex);
						float externalTemp = (float)frame.getData(0);
						std::cout << "  -> [CLIMATE] External temperature: " << externalTemp << "C" << std::endl;
					}
				}
			}
		}

		simulateTemperatureChange();

		turnAc();

		std::lock_guard<std::mutex> lock(tempMutex);
		std::cout << "[CLIMATE DEBUG] Current: " << currentTemp << "C | Target: " << targetTemp
			<< "C | AC: " << (acOn ? "ON" : "OFF") << " | Vent: " << ventSpeed << std::endl;

		sendClimateStatus();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void ClimateControlECU::simulateTemperatureChange() {
	std::lock_guard<std::mutex> lock(tempMutex);

	float tempDiff = targetTemp - currentTemp;

	if (acOn) {
		float changeRate = (ventSpeed / 8.0f) * 1.5f;

		if (tempDiff > 0.1f) {
			currentTemp += changeRate;
			if (currentTemp > targetTemp) currentTemp = targetTemp;
		}
		else if (tempDiff < -0.1f) {
			currentTemp -= changeRate;
			if (currentTemp < targetTemp) currentTemp = targetTemp;
		}
	}
	else {
		float exteriorTemp = 28.0f;
		float approachRate = 0.1f;

		if (currentTemp < exteriorTemp) {
			currentTemp += approachRate;
		}
		else if (currentTemp > exteriorTemp) {
			currentTemp -= approachRate;
		}
	}
}

void ClimateControlECU::calculateAirDistribution() {
	float tempDiff = currentTemp - targetTemp;
	float absDiff = std::abs(tempDiff);

	airDistribution.clear();

	if (absDiff >= 1.0f) {
		acOn = true;

		if (absDiff >= 5.0f) ventSpeed = 8;
		else if (absDiff >= 3.0f) ventSpeed = 5;
		else ventSpeed = 2;

		if (tempDiff > 0) {  
			airDistribution.push_back("Face");
			if (absDiff > 4.0f) {
				airDistribution.push_back("Feet");
			}
		}
		else {  
			airDistribution.push_back("Feet");
			if (absDiff > 3.0f) {
				airDistribution.push_back("Windshield");
			}
			if (absDiff > 6.0f) {
				airDistribution.push_back("Face");
			}
		}
	}
	else {
		acOn = false;
		ventSpeed = 0;
		airDistribution.push_back("None");
	}
}

void ClimateControlECU::turnAc() {
	calculateAirDistribution();
}

void ClimateControlECU::sendClimateStatus() {
	CANFrame frame;
	frame.setId(0x701);  
	frame.setData({ (uint8_t)currentTemp, (uint8_t)ventSpeed });
	bus.send(this, frame);
}

void ClimateControlECU::displayStatus() {
	std::lock_guard<std::mutex> lock(tempMutex);

	std::cout << "[CLIMATE] Temp: " << currentTemp << "C | Target: " << targetTemp << "C" << std::endl;
	std::cout << "          AC: " << (acOn ? "ON" : "OFF") << " | Vent: " << ventSpeed << std::endl;
	std::cout << "          Air to: ";
	for (const auto& area : airDistribution) {
		std::cout << area << " ";
	}
	std::cout << std::endl;
}

void ClimateControlECU::setTargetTemp(float temp) {
	std::lock_guard<std::mutex> lock(tempMutex);
	targetTemp = temp;
	std::cout << "[CLIMATE] Target temperature set to: " << temp << "C" << std::endl;
}

void ClimateControlECU::shutdown() {
	running = false;
	cv.notify_one();
	if (climateThread.joinable()) {
		climateThread.join();
	}
}