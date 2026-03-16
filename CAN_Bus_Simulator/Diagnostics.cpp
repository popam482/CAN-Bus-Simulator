#include "Diagnostics.h"
#include <iostream>
#include <chrono>

Diagnostics::Diagnostics(CANBus& b)
	: bus(b), oilTemp(0.0f), oilLevel(0.0f), coolantTemp(0.0f),
	coolantLevel(0.0f), fuelLevel(0.0f), running(true) {

	diagnosticThread = std::thread(&Diagnostics::diagnosticWorker, this);
}

Diagnostics::~Diagnostics() {
	shutdown();
}

std::string Diagnostics::getName() {
	return "DiagnosticECU";
}

void Diagnostics::receiveFrame(CANFrame& frame) {
	{
		std::lock_guard<std::mutex> lock(frameMutex);
		frameQueue.push(frame);
	}
	cv.notify_one();
}

void Diagnostics::diagnosticWorker() {
	int checkCounter = 0;

	while (running) {
		std::unique_lock<std::mutex> lock(frameMutex);
		cv.wait_for(lock, std::chrono::milliseconds(500),
			[this] { return !frameQueue.empty() || !running; });

		if (!frameQueue.empty()) {
			CANFrame frame = frameQueue.front();
			frameQueue.pop();
			lock.unlock();

			if (frame.getId() == 0x104) {
				oilTemp = frame.getData(0);
				oilLevel = frame.getData(1) / 100.0f;
			}
			else if (frame.getId() == 0x105) {
				coolantTemp = frame.getData(0);
				coolantLevel = frame.getData(1) / 100.0f;
			}
			else if (frame.getId() == 0x103) {
				fuelLevel = frame.getData(0) / 100.0f;
			}
		}

		checkCounter++;
		if (checkCounter >= 3) {
			checkFaults();
			checkCounter = 0;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Diagnostics::checkFaults() {
	std::lock_guard<std::mutex> lock(diagMutex);

	activeFaults.clear();

	if (oilTemp > 105.0f) {
		activeFaults.insert(FaultCode::OIL_TEMP_HIGH);
	}

	if (oilLevel < 0.20f) {
		activeFaults.insert(FaultCode::OIL_LEVEL_LOW);
	}

	if (coolantTemp > 115.0f) {
		activeFaults.insert(FaultCode::COOLANT_TEMP_HIGH);
	}

	if (coolantLevel < 0.20f) {
		activeFaults.insert(FaultCode::COOLANT_LEVEL_LOW);
	}

	if (fuelLevel < 0.10f) {
		activeFaults.insert(FaultCode::FUEL_LEVEL_LOW);
	}
}

std::string Diagnostics::getFaultDescription(FaultCode fault) {
	switch (fault) {
	case FaultCode::OIL_TEMP_HIGH:
		return "Oil Temperature High";
	case FaultCode::OIL_LEVEL_LOW:
		return "Oil Level Low";
	case FaultCode::COOLANT_TEMP_HIGH:
		return "Coolant Temperature High";
	case FaultCode::COOLANT_LEVEL_LOW:
		return "Coolant Level Low";
	case FaultCode::FUEL_LEVEL_LOW:
		return "Fuel Level Low";
	case FaultCode::NO_FAULT:
		return "No Faults";
	default:
		return "Unknown Fault";
	}
}

void Diagnostics::displayFaults() {
	std::lock_guard<std::mutex> lock(diagMutex);

	std::cout << "\n[ACTIVE FAULTS]" << std::endl;
	if (activeFaults.empty()) {
		std::cout << "   No active faults detected\n" << std::endl;
	}
	else {
		int i = 1;
		for (auto& fault : activeFaults) {
			std::cout << "  " << i++ << ". " << getFaultDescription(fault) << std::endl;
		}
	}
}

void Diagnostics::exportToFile(const std::string& filename) {
	std::ofstream file(filename);
	file << "=== DIAGNOSTIC REPORT ===" << std::endl;
	file << "Oil Temp: " << oilTemp << "C" << std::endl;
	file << "Oil Level: " << (oilLevel * 100) << "%" << std::endl;
	file << "Coolant Temp: " << coolantTemp << "C" << std::endl;
	file << "Coolant Level: " << (coolantLevel * 100) << "%" << std::endl;
	file << "Fuel Level: " << (fuelLevel * 100) << "%" << std::endl;
	file.close();
	std::cout << "[DIAGNOSTICS] Diagnostic report successfully written\n";
}

void Diagnostics::shutdown() {
	running = false;
	cv.notify_one();
	if (diagnosticThread.joinable()) {
		diagnosticThread.join();
	}
}