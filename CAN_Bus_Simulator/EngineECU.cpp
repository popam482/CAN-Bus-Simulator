#include "EngineECU.h" 

#include <iostream>
#include <string>


EngineECU::EngineECU(CANBus& b) : bus(b), running(true), fuelTankLevel(0.8f) {
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
		" | Oil level: " << oil.level <<  std::endl;
}

void EngineECU::getCoolantData()
{
	std::cout << "Coolant temperature : " << coolant.temperature <<
		" | Coolant level: " << coolant.level<<std::endl;
}

void EngineECU::getFuelData()
{
	std::cout << "Fuel tank level: " << (fuelTankLevel * 100) << "% full | Fuel Consumption: "
		<< fuelConsumption << "/100km\n";
}

void EngineECU::getRPMData()
{
	std::cout << " Current speed: " << currentSpeed << " at " << engineRPM << " RPM\n";
}

void EngineECU::calculateRPM(uint8_t speed) {
	
	if (speed == 0) {
		engineRPM = 800;
	}
	else {
		engineRPM = 800 + (speed * 80);
	}

	if (engineRPM > 7000) engineRPM = 7000;
}

void EngineECU::updateFuelLevel(uint8_t speed) {
	if (speed > 0) {
		fuelTankLevel -= (speed * 0.00001f);
	}

	if (fuelTankLevel < 0.0f) fuelTankLevel = 0.0f;
	if (fuelTankLevel > 1.0f) fuelTankLevel = 1.0f;
}

void EngineECU::calculateFuelConsumption(uint8_t speed) {

	if (speed == 0) {
		fuelConsumption = 0.0f;  
	}
	else if (speed < 40) {
		fuelConsumption = 10.0f;  
	}
	else if (speed < 100) {
		fuelConsumption = 6.5f;   
	}
	else {
		fuelConsumption = 8.5f;
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

			currentSpeed = speed;
			calculateRPM(speed);
			calculateFuelConsumption(speed);
			updateFluidLevels(speed);

			CANFrame frame;
			frame.setId(0x101);
			frame.setData({ speed });
			bus.send(this, frame);

			CANFrame rpmFrame;
			rpmFrame.setId(0x102);
			uint8_t rpmHigh = (engineRPM >> 8) & 0xFF;
			uint8_t rpmLow = engineRPM & 0xFF;
			rpmFrame.setData({ rpmHigh, rpmLow });
			bus.send(this, rpmFrame);

			CANFrame fuelFrame;
			fuelFrame.setId(0x103);
			uint8_t fuelPercent = static_cast<uint8_t>(fuelTankLevel * 100);
			fuelFrame.setData({ fuelPercent });
			bus.send(this, fuelFrame);

			CANFrame oilFrame;
			oilFrame.setId(0x104);
			uint8_t oilTemp = static_cast<uint8_t>(oil.temperature);
			uint8_t oilLevel = static_cast<uint8_t>(oil.level * 100);
			oilFrame.setData({ oilTemp, oilLevel });
			bus.send(this, oilFrame);

			CANFrame coolantFrame;
			coolantFrame.setId(0x105);
			uint8_t coolantTemp = static_cast<uint8_t>(coolant.temperature);
			uint8_t coolantLevel = static_cast<uint8_t>(coolant.level * 100);
			coolantFrame.setData({ coolantTemp, coolantLevel });
			bus.send(this, coolantFrame);
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