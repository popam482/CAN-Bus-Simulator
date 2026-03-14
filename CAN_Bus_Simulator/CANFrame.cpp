#include "CANFrame.h"

void CANFrame::print() {
    std::cout << "[" << std::setw(7) << timestamp << "us]"
        << " ID:0x" << std::hex << std::uppercase << id
        << " DLC:" << std::dec << (int)dlc
        << " Data:";
    for (auto b : data)
        std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << std::dec << std::endl;
}

uint32_t CANFrame::getId() {
    return id;
}

void CANFrame::setId(uint32_t id) {
    this->id = id;
}

void CANFrame::setData(std::vector<uint8_t> data) {
    this->data = data;
    dlc = static_cast<uint8_t>(data.size());

    if (dlc > 8) {
        dlc = 8;
        data.resize(8);
    }
}

uint8_t CANFrame::getData(int idx) {
    return data[idx];
}