#include "TransmissionECU.h"


std::string TransmissionECU::getName() {
	return "TransmisionECU";
}

void TransmissionECU::receiveFrame(CANFrame& frame) {
    if (frame.getId() == 0x101) {
        uint8_t speed = frame.getData(0);
        uint8_t newGear = (speed / 20) + 1;
        if (newGear > 6) newGear = 6;

        if (newGear != currentGear) {
            currentGear = newGear;
            std::cout << "  -> [TRANSMISSION] Shifting to Gear: " << (int)currentGear << std::endl;
            sendCurrentGear();
        }
    }
    if (frame.getId() == 0x201) {
        std::cout << "  -> [TRANSMISSION] Brake detected. Preparing to downshift..." << std::endl;
    }
}

void TransmissionECU::sendCurrentGear() {
    CANFrame frame;
    frame.setId(0x301);
    frame.setData({currentGear});
    bus.send(this, frame);
}