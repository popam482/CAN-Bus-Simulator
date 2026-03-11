#include "EngineECU.h"

#include <iostream>
#include <string>


std::string EngineECU::getName(){
	return { "EngineECU" };
}

void EngineECU::sendSpeed(uint8_t speed) {
    CANFrame frame;
    frame.setId(0x101); 
    frame.setData({speed});
    bus.send(this, frame);
}

void EngineECU::receiveFrame(CANFrame &frame) {
    if (frame.getId() == 0x201) {
        std::cout << "  -> Engine ECU received BRAKE signal. Reducing speed..." << std::endl;
    }
}