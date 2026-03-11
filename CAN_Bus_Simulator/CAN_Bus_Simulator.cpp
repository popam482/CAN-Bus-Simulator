#include <iostream>
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
    engine.sendSpeed(85);
    brakes.pressBrake();
    engine.sendSpeed(70);

    return 0;
}