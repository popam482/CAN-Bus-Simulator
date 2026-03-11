#include "CANFrame.h"

void CANFrame::print() {
    std::cout << "ID: 0x" << std::hex << std::uppercase << id << " | Data: ";
    for (auto b : data) std::cout << std::setw(2) << std::setfill('0') << (int)b << " ";
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
}