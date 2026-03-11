#include "DashboardECU.h"


std::string DashboardECU::getName(){
    return "Dashboard_Display";
}

void DashboardECU::receiveFrame(CANFrame& frame){
    if (frame.getId() == 0x101) {
        std::cout << "  -> [DASHBOARD] Speedometer update: " << (int)frame.getData(0) << " km/h" << std::endl;
    }
}