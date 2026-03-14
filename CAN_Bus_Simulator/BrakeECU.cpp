#include "BrakeECU.h"
#include <chrono>

BrakeECU::BrakeECU(CANBus& b) : bus(b), brakePressured(false), running(true) {
	brakeThread = std::thread(&BrakeECU::brakeWorker, this);
}

BrakeECU::~BrakeECU() {
	shutdown();
}

std::string BrakeECU::getName() {
	return "Brake_Sensor_ECU";
}

void BrakeECU::pressBrake() {
	brakePressured = true;
}

void BrakeECU::brakeWorker() {
	while (running) {
		if (brakePressured) {
			std::cout << "\n[DRIVER] Pressed the brake pedal!" << std::endl;

			CANFrame frame;
			frame.setId(0x201);
			frame.setData({ 1 });
			bus.send(this, frame);

			brakePressured = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void BrakeECU::receiveFrame(CANFrame& frame) {
	//no messages from other nodes
}

void BrakeECU::shutdown() {
	running = false;
	if (brakeThread.joinable()) {
		brakeThread.join();
	}
}