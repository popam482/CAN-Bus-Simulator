#include "EngineECU.h"

#include <iostream>
#include <string>


EngineECU::EngineECU(CANBus& b) : bus(b), running(true) {
	senderThread = std::thread(&EngineECU::senderWorker, this);
}

EngineECU::~EngineECU() {
	shutdown();
}

std::string EngineECU::getName(){
	return { "EngineECU" };
}

void EngineECU::sendSpeed(uint8_t speed) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(speed);
    cv.notify_one();
}

void EngineECU::receiveFrame(CANFrame &frame) {
    if (frame.getId() == 0x201) {
        std::cout << "  -> Engine ECU received BRAKE signal. Reducing speed..." << std::endl;
    }
}

void EngineECU::senderWorker() {
	while (running) {
		std::unique_lock<std::mutex> lock(queueMutex);
		cv.wait(lock, [this] { return !messageQueue.empty() || !running; });

		if (!messageQueue.empty()) {
			uint8_t speed = messageQueue.front();
			messageQueue.pop();
			lock.unlock();

			CANFrame frame;
			frame.setId(0x101);
			frame.setData({ speed });
			bus.send(this, frame);
		}
	}
}

void EngineECU::shutdown() {
	running = false;
	cv.notify_one();
	if (senderThread.joinable()) {
		senderThread.join();
	}
}