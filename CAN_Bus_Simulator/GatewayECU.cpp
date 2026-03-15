#include "GatewayECU.h"
#include <iostream>
#include <chrono>

GatewayECU::GatewayECU(CANBus& pBus, CANBus& cBus)
    : powertrainBus(pBus), comfortBus(cBus), running(true) {

    setupRoutingRules();

    ptrainMonitorThread = std::thread(&GatewayECU::monitorPowertrainBus, this);
    comfortMonitorThread = std::thread(&GatewayECU::monitorComfortBus, this);
}

GatewayECU::~GatewayECU() {
    shutdown();
}

std::string GatewayECU::getName() {
    return "Gateway_ECU";
}

void GatewayECU::setupRoutingRules() {
    powertrainToComfortMessages.insert(0x101);
    powertrainToComfortMessages.insert(0x201);  
    powertrainToComfortMessages.insert(0x301);  
    powertrainToComfortMessages.insert(0x401);  

    std::cout << "\n[GATEWAY] Routing configured:" << std::endl;
    std::cout << "  Powertrain -> Comfort: Speed(0x101), Brake(0x201), Gear(0x301), ABS(0x401)" << std::endl;
    std::cout << "  Comfort -> Powertrain: (none)\n" << std::endl;
}

void GatewayECU::receiveFrame(CANFrame& frame) {
    {
        std::lock_guard<std::mutex> lock(ptrainMutex);
        powertrainQueue.push(frame);
    }
    cvPtrain.notify_one();
}

void GatewayECU::monitorPowertrainBus() {
    std::cout << "[GATEWAY] Powertrain Bus monitor started" << std::endl;

    while (running) {
        std::unique_lock<std::mutex> lock(ptrainMutex);
        cvPtrain.wait_for(lock, std::chrono::milliseconds(500),
            [this] { return !powertrainQueue.empty() || !running; });

        if (!powertrainQueue.empty()) {
            CANFrame frame = powertrainQueue.front();
            powertrainQueue.pop();
            lock.unlock();

            if (powertrainToComfortMessages.count(frame.getId())) {
                std::cout << "[GATEWAY] Routing 0x" << std::hex << frame.getId()
                    << std::dec << " Powertrain -> Comfort Bus" << std::endl;
                comfortBus.send(this, frame);
            }
        }
    }
}

void GatewayECU::monitorComfortBus() {
    std::cout << "[GATEWAY] Comfort Bus monitor started" << std::endl;

    while (running) {
        std::unique_lock<std::mutex> lock(comfortMutex);
        cvComfort.wait_for(lock, std::chrono::milliseconds(500),
            [this] { return !comfortQueue.empty() || !running; });

        if (!comfortQueue.empty()) {
            CANFrame frame = comfortQueue.front();
            comfortQueue.pop();
            lock.unlock();
            if (comfortToPowertrainMessages.count(frame.getId())) {
                std::cout << "[GATEWAY] Routing 0x" << std::hex << frame.getId()
                    << std::dec << " Comfort -> Powertrain Bus" << std::endl;
                powertrainBus.send(this, frame);
            }
        }
    }
}

void GatewayECU::shutdown() {
    running = false;
    cvPtrain.notify_one();
    cvComfort.notify_one();

    if (ptrainMonitorThread.joinable()) {
        ptrainMonitorThread.join();
    }
    if (comfortMonitorThread.joinable()) {
        comfortMonitorThread.join();
    }

    std::cout << "[GATEWAY] Shutdown complete" << std::endl;
}