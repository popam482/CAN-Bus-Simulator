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
#include "BcmECU.h"
#include "GatewayECU.h"

int main() {
    CANBus vehicleBus;
    CANBus bodyBus;

    EngineECU engine(vehicleBus);
    DashboardECU dashboard;
    BrakeECU brakes(vehicleBus);
    TransmissionECU transmission(vehicleBus);
    AbsECU abs(vehicleBus);
    ClimateControlECU climate(bodyBus, 22.0f, 28.0f);
    BcmECU bcm(bodyBus);


    vehicleBus.connectNode(&engine);
    bodyBus.connectNode(&dashboard);
    vehicleBus.connectNode(&brakes);
    vehicleBus.connectNode(&transmission);
    vehicleBus.connectNode(&abs);
    bodyBus.connectNode(&climate);
    bodyBus.connectNode(&bcm);

    GatewayECU gateway(vehicleBus, bodyBus);
    vehicleBus.connectNode(&gateway);
    bodyBus.connectNode(&gateway);

    std::cout << "\n--- Starting Simulation ---\n" << std::endl;

    bcm.lockDoors();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bcm.turnHeadLights(true);
    bcm.turnFogLights(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getFuelData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getRPMData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.sendSpeed(50);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getCoolantData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getOilData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bcm.isRaining(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    climate.setTargetTemp(20.0f);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    climate.displayStatus();

    engine.sendSpeed(85);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getFuelData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getRPMData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    climate.setTargetTemp(22.5f);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    climate.displayStatus();

    engine.sendSpeed(70);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    bcm.displayStatus();

    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.sendSpeed(30);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getFuelData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getRPMData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bcm.isRaining(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    climate.setTargetTemp(21.0f);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    climate.displayStatus();

    bcm.unlockDoors();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getCoolantData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.getOilData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bcm.displayStatus();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.shutdown();
    transmission.shutdown();
    dashboard.shutdown();
    brakes.shutdown();

    std::cout << "\n--- Simulation ended ---\n" << std::endl;

    return 0;
}