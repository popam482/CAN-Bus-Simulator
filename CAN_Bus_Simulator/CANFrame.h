#pragma once

#include <iostream>
#include <iomanip>
#include <vector>

class CANFrame
{
private:
	uint32_t id;
	std::vector<uint8_t> data;
	uint8_t  dlc;      
	uint64_t timestamp;  

public:

	CANFrame() :id(0), dlc(0){}

	void print();

	uint32_t getId();

	void setId(uint32_t id);

	void setData(std::vector<uint8_t> data);

	uint8_t getData(int idx);

	uint8_t getDlc();

	uint64_t getTimestamp();
};

