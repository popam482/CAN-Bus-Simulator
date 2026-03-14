#include <iostream>
#include <thread>
#include <chrono>
#include "CANBus.h"
#include "EngineECU.h"
#include "DashboardECU.h"
#include "BrakeECU.h"
#include "TransmissionECU.h"
#include "AbsECU.h"
#include "ClimateControlECU.h"

int main() {
    CANBus vehicleBus;

    EngineECU engine(vehicleBus);
    DashboardECU dashboard;
    BrakeECU brakes(vehicleBus);
    TransmissionECU transmission(vehicleBus);
    AbsECU abs(vehicleBus);
    ClimateControlECU climate(vehicleBus, 22.0f, 28.0f);

    vehicleBus.connectNode(&engine);
    vehicleBus.connectNode(&dashboard);
    vehicleBus.connectNode(&brakes);
    vehicleBus.connectNode(&transmission);
    vehicleBus.connectNode(&abs);
    vehicleBus.connectNode(&climate);

    std::cout << "\n--- Starting Simulation ---\n" << std::endl;

    engine.sendSpeed(50);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    climate.setTargetTemp(20.0f);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    climate.displayStatus();

    engine.sendSpeed(85);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    climate.setTargetTemp(22.5f);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    climate.displayStatus();

    engine.sendSpeed(70);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.sendSpeed(30);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    climate.setTargetTemp(21.0f);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    climate.displayStatus();

    engine.shutdown();
    transmission.shutdown();
    dashboard.shutdown();
    brakes.shutdown();

    std::cout << "\n--- Simulation ended ---\n" << std::endl;

    return 0;
}