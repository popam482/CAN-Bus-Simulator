#include "AbsECU.h"

AbsECU::AbsECU(CANBus& b) : bus(b), wheelSpeed(0), absActivated(false), running(true) {
    processorThread = std::thread(&AbsECU::processorWorker, this);
}

AbsECU::~AbsECU() {
    shutdown();
}

std::string AbsECU::getName() {
    return "ABS_ECU";
}

void AbsECU::receiveFrame(CANFrame& frame) {
    {
        std::lock_guard<std::mutex> lock(frameMutex);
        frameQueue.push(frame);
    }
    cv.notify_one();
}

void AbsECU::processorWorker() {
    while (running) {
        std::unique_lock<std::mutex> lock(frameMutex);
        cv.wait(lock, [this] { return !frameQueue.empty() || !running; });

        if (!frameQueue.empty()) {
            CANFrame frame = frameQueue.front();
            frameQueue.pop();
            lock.unlock();

            if (frame.getId() == 0x201) {
                wheelSpeed = frame.getData(0);

                if (wheelSpeed > 80 || (frame.getId() == 0x201 && wheelSpeed < 10))
                    absActivated = true;

                else if (wheelSpeed < 20) {  
                    std::cout << "  -> [ABS] Wheel lock detected! Activating ABS..." << std::endl;
                    applyABS();
                    absActivated = true;
                }
            }
        }
    }
}

void AbsECU::applyABS() {
    CANFrame frame;
    frame.setId(0x401);  
    frame.setData({ 1 });
    bus.send(this, frame);
}

bool AbsECU::isActivated(){ return absActivated; }
uint8_t AbsECU::getWheelSpeed(){ return wheelSpeed; }
void AbsECU::setWheelSpeed(uint8_t speed) { wheelSpeed=speed;}

void AbsECU::shutdown() {
    running = false;
    cv.notify_one();
    if (processorThread.joinable()) {
        processorThread.join();
    }
}