#include "BrakeECU.h"


std::string BrakeECU::getName(){ 
    return "Brake_Sensor_ECU";
}

void BrakeECU::pressBrake() {
    CANFrame frame;
    frame.setId(0x201); 
    frame.setData({ 1 }); 
    std::cout << "\n[DRIVER] Pressed the brake pedal!" << std::endl;
    bus.send(this, frame);
}

void BrakeECU::receiveFrame(CANFrame& frame) {

}