#include "TransmissionECU.h"

TransmissionECU::TransmissionECU(CANBus& b) : bus(b), currentGear(0), running(true) {
	processorThread = std::thread(&TransmissionECU::processorWorker, this);
}

TransmissionECU::~TransmissionECU() {
	shutdown();
}

std::string TransmissionECU::getName() {
	return "TransmissionECU";
}

void TransmissionECU::receiveFrame(CANFrame& frame) {
	{
		std::lock_guard<std::mutex> lock(frameMutex);
		frameQueue.push(frame);
	}
	cv.notify_one();
}

void TransmissionECU::processorWorker() {
	while (running) {
		std::unique_lock<std::mutex> lock(frameMutex);
		cv.wait(lock, [this] { return !frameQueue.empty() || !running; });

		if (!frameQueue.empty()) {
			CANFrame frame = frameQueue.front();
			frameQueue.pop();
			lock.unlock();

			if (frame.getId() == 0x101) {
				uint8_t speed = frame.getData(0);
				uint8_t newGear;

				if (speed == 0) {
					newGear = 0;  
				}
				else if (speed <= 20) {
					newGear = 1;
				}
				else if (speed <= 40) {
					newGear = 2;
				}
				else if (speed <= 60) {
					newGear = 3;
				}
				else if (speed <= 80) {
					newGear = 4;
				}
				else {
					newGear = 5;
				}

				if (newGear != currentGear) {
					currentGear = newGear;
					std::cout << "  -> [TRANSMISSION] Shifting to Gear: " << (int)currentGear << std::endl;
					sendCurrentGear();
				}
			}
			if (frame.getId() == 0x201) {
				std::cout << "  -> [TRANSMISSION] Brake detected. Preparing to downshift..." << std::endl;

				if (currentGear > 0){
					currentGear--;
					sendCurrentGear();
				}
			}
		}
	}
}

void TransmissionECU::sendCurrentGear() {
	CANFrame frame;
	frame.setId(0x301);
	frame.setData({ currentGear });
	bus.send(this, frame);
}

uint8_t TransmissionECU::getCurrentGear() {
	return currentGear;
}

void TransmissionECU::shutdown() {
	running = false;
	cv.notify_one();
	if (processorThread.joinable()) {
		processorThread.join();
	}
}