#include <iostream>
#include "CANBus.h"
#include "EngineECU.h"
#include "DashboardECU.h"
#include "BrakeECU.h"

int main() {
    CANBus vehicleBus;

    EngineECU engine(vehicleBus);
    DashboardECU dashboard;
    BrakeECU brakes(vehicleBus);

    vehicleBus.connectNode(&engine);
    vehicleBus.connectNode(&dashboard);
    vehicleBus.connectNode(&brakes);

    std::cout << "\n--- Starting Simulation ---\n" << std::endl;

    engine.sendSpeed(50);
    engine.sendSpeed(85);
    brakes.pressBrake();
    engine.sendSpeed(70);

    return 0;
}