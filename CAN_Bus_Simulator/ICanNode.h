#pragma once

#include <iostream>
#include "CANFrame.h"

class ICanNode
{
public:
    virtual void receiveFrame(CANFrame& frame) = 0;
    virtual std::string getName() = 0;

};

