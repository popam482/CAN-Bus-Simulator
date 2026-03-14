#include <iostream>
#include <thread>
#include <chrono>
#include "CANBus.h"
#include "EngineECU.h"
#include "DashboardECU.h"
#include "BrakeECU.h"
#include "TransmissionECU.h"

int main() {
    CANBus vehicleBus;

    EngineECU engine(vehicleBus);
    DashboardECU dashboard;
    BrakeECU brakes(vehicleBus);
    TransmissionECU transmission(vehicleBus);

    vehicleBus.connectNode(&engine);
    vehicleBus.connectNode(&dashboard);
    vehicleBus.connectNode(&brakes);
    vehicleBus.connectNode(&transmission);

    std::cout << "\n--- Starting Simulation ---\n" << std::endl;

    engine.sendSpeed(50);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.sendSpeed(85);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.sendSpeed(70);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.sendSpeed(30);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.shutdown();
    transmission.shutdown();
    dashboard.shutdown();
    brakes.shutdown();

    std::cout << "\n--- Simulation ended ---\n" << std::endl;

    return 0;
}