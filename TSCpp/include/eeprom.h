
#pragma once

#include <iostream>
#include <fstream>

class DEEPROM
{
public:
	DEEPROM();
	~DEEPROM();

	void write(int address, uint8_t value);
	uint8_t read(int address);

private:
	std::fstream eepromFile;
};
