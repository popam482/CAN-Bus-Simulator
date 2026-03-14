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
    bool isActivated;

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
    void shutdown();
};