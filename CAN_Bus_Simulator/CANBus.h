#pragma once

#include <iostream>
#include <vector>
#include "ICanNode.h"

class CANBus
{
private:
	std::vector <ICanNode*> nodes;

public:

	void connectNode(ICanNode* node);

	void send(ICanNode * sender, CANFrame &frame);

};

