#include "TransmissionECU.h"

TransmissionECU::TransmissionECU(CANBus& b) : bus(b), currentGear(1), running(true) {
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
				uint8_t newGear = (speed / 20) + 1;
				if (newGear > 6) newGear = 6;

				if (newGear != currentGear) {
					currentGear = newGear;
					std::cout << "  -> [TRANSMISSION] Shifting to Gear: " << (int)currentGear << std::endl;
					sendCurrentGear();
				}
			}
			if (frame.getId() == 0x201) {
				std::cout << "  -> [TRANSMISSION] Brake detected. Preparing to downshift..." << std::endl;
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

void TransmissionECU::shutdown() {
	running = false;
	cv.notify_one();
	if (processorThread.joinable()) {
		processorThread.join();
	}
}