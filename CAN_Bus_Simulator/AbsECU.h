#pragma once

#include "CANBus.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class AbsECU : public ICanNode {
private:
    CANBus& bus;
    uint8_t wheelSpeed;
    bool absActivated;

    std::queue<CANFrame> frameQueue;
    std::mutex frameMutex;
    std::condition_variable cv;
    std::thread processorThread;
    std::atomic<bool> running;

    void processorWorker();
    void applyABS();

public:
    AbsECU(CANBus& b);
    ~AbsECU();

    std::string getName() override;
    void receiveFrame(CANFrame& frame) override;

    bool isActivated();
    uint8_t getWheelSpeed();
    void setWheelSpeed(uint8_t speed);
    void shutdown();
};