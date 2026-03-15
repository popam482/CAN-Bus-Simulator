#pragma once

#include "CANBus.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <set>
#include <queue>
#include <condition_variable>

class GatewayECU : public ICanNode {
private:
    CANBus& powertrainBus;
    CANBus& comfortBus;

    std::queue<CANFrame> powertrainQueue;
    std::queue<CANFrame> comfortQueue;
    std::mutex ptrainMutex;
    std::mutex comfortMutex;
    std::condition_variable cvPtrain;
    std::condition_variable cvComfort;

    std::thread ptrainMonitorThread;
    std::thread comfortMonitorThread;
    std::atomic<bool> running;

    std::set<uint32_t> powertrainToComfortMessages;
    std::set<uint32_t> comfortToPowertrainMessages;

    void monitorPowertrainBus();
    void monitorComfortBus();
    void setupRoutingRules();

public:
    GatewayECU(CANBus& pBus, CANBus& cBus);
    ~GatewayECU();

    std::string getName() override;
    void receiveFrame(CANFrame& frame) override;
    void shutdown();
};