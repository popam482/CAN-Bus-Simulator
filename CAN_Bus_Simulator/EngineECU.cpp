#include "EngineECU.h" 

#include <iostream>
#include <string>


EngineECU::EngineECU(CANBus& b) : bus(b), running(true) {
	oil.temperature = 20.0f;
	oil.level = 0.85f;
	coolant.temperature = 20.0f;
	coolant.level = 0.75f;
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

void EngineECU::getOilData()
{
	std::cout<<"Oil temperature : " << oil.temperature <<
		" | Oil level: " << oil.level;
}

void EngineECU::getCoolantData()
{
	std::cout << "Coolant temperature : " << coolant.temperature <<
		" | Coolant level: " << coolant.level;
}

void EngineECU::senderWorker() {
	while (running) {
		std::unique_lock<std::mutex> lock(queueMutex);
		cv.wait(lock, [this] { return !messageQueue.empty() || !running; });

		if (!messageQueue.empty()) {
			uint8_t speed = messageQueue.front();
			messageQueue.pop();
			lock.unlock();

			currentSpeed = speed;
			updateFluidLevels(speed);

			CANFrame frame;
			frame.setId(0x101);
			frame.setData({ speed });
			bus.send(this, frame);
		}
	}
}

void EngineECU::updateFluidLevels(uint8_t speed)
{
	coolant.temperature = 80.0f + (speed * 0.3f);
	oil.temperature = 70.0f + (speed * 0.35f);

	if (coolant.temperature > 120.0f) coolant.temperature = 120.0f;
	if (oil.temperature > 110.0f) oil.temperature = 110.0f;

	if (speed > 50) {
		coolant.level -= 0.0005f;
		oil.level -= 0.0003f;
	}

	if (coolant.level < 0.0f) coolant.level = 0.0f;
	if (oil.level < 0.0f) oil.level = 0.0f;
	if (coolant.level > 1.0f) coolant.level = 1.0f;
	if (oil.level > 1.0f) oil.level = 1.0f;
}

void EngineECU::shutdown() {
	running = false;
	cv.notify_one();
	if (senderThread.joinable()) {
		senderThread.join();
	}
}