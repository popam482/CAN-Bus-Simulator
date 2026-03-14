#include "DashboardECU.h"
#include <chrono>

DashboardECU::DashboardECU() : running(true) {
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
			}
			if (frame.getId() == 0x301) {
				std::cout << "  -> [DASHBOARD] Current Gear: " << (int)frame.getData(0) << std::endl;
			}
		}
	}
}

void DashboardECU::shutdown() {
	running = false;
	cv.notify_one();
	if (displayThread.joinable()) {
		displayThread.join();
	}
}