#pragma once

#include <iostream>
#include <vector>
#include "ICanNode.h"
#include <queue>
#include <mutex>

class CANBus
{
private:
	std::vector <ICanNode*> nodes;
	
	mutable std::mutex nodesMutex;

public:

	void connectNode(ICanNode* node);

	void send(ICanNode * sender, CANFrame &frame);

};

