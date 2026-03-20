#include "DashboardECU.h"
#include <chrono>

DashboardECU::DashboardECU() : displayedSpeed(0), displayedRPM(0), displayedFuelLevel(0),
displayedOilTemp(0), displayedCoolantTemp(0), running(true){
	displayThread = std::thread(&DashboardECU::displayWorker, this);
}

DashboardECU::~DashboardECU() {
	shutdown();
}

std::string DashboardECU::getName() {
	return "Dashboard_Display";
}

void DashboardECU::receiveFrame(CANFrame& frame) {
	{
		std::lock_guard<std::mutex> lock(displayMutex);
		displayQueue.push(frame);
	}
	cv.notify_one();
}

void DashboardECU::displayWorker() {
	while (running) {
		std::unique_lock<std::mutex> lock(displayMutex);
		cv.wait(lock, [this] { return !displayQueue.empty() || !running; });

		if (!displayQueue.empty()) {
			CANFrame frame = displayQueue.front();
			displayQueue.pop();
			lock.unlock();

			if (frame.getId() == 0x101) {
				std::cout << "  -> [DASHBOARD] Speedometer update: " << (int)frame.getData(0) << " km/h" << std::endl;
				displayedSpeed = frame.getData(0);
			}
			if (frame.getId() == 0x301) {
				std::cout << "  -> [DASHBOARD] Current Gear: " << (int)frame.getData(0) << std::endl;
			}

			if (frame.getId() == 0x102) {  // RPM
				uint8_t rpmHigh = frame.getData(0);
				uint8_t rpmLow = frame.getData(1);
				displayedRPM = (rpmHigh << 8) | rpmLow;
			}

			if (frame.getId() == 0x103) {  // Fuel
				displayedFuelLevel = frame.getData(0);
			}

			if (frame.getId() == 0x104) {  // Oil Temp
				displayedOilTemp = frame.getData(0);
			}

			if (frame.getId() == 0x105) {  // Coolant Temp
				displayedCoolantTemp = frame.getData(0);
			}
		}
	}
}

uint8_t DashboardECU::getDisplayedSpeed() { return displayedSpeed; }
uint16_t DashboardECU::getDisplayedRPM() { return displayedRPM; }
uint8_t DashboardECU::getDisplayedFuelLevel() { return displayedFuelLevel; }
uint8_t DashboardECU::getDisplayedOilTemp() { return displayedOilTemp; }
uint8_t DashboardECU::getDisplayedCoolantTemp() { return displayedCoolantTemp; }

void DashboardECU::setDisplayedSpeed(uint8_t speed) { displayedSpeed = speed; }
void DashboardECU::setDisplayedRPM(uint16_t rpm) { displayedRPM = rpm; }
void DashboardECU::setDisplayedFuelLevel(uint8_t fuel) { displayedFuelLevel = fuel; }
void DashboardECU::setDisplayedOilTemp(uint8_t temp) { displayedOilTemp = temp; }
void DashboardECU::setDisplayedCoolantTemp(uint8_t temp) { displayedCoolantTemp = temp; }

void DashboardECU::shutdown() {
	running = false;
	cv.notify_one();
	if (displayThread.joinable()) {
		displayThread.join();
	}
}