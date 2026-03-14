#include "CANBus.h"

void CANBus::connectNode(ICanNode* node) {
    std::lock_guard<std::mutex> lock(nodesMutex);
    nodes.push_back(node);
    std::cout << "[Bus] Node " << node->getName() << " connected." << std::endl;
}

void CANBus::send(ICanNode* sender, CANFrame& frame) {
    std::lock_guard<std::mutex> lock(nodesMutex);
    std::cout << "[Bus] " << sender->getName() << " sent message 0x" << std::hex << frame.getId() << std::dec << std::endl;
    for (auto node : nodes) {
        if (node != sender) {
            node->receiveFrame(frame);
        }
    }
}