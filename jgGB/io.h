#pragma once
#include "common.h"

class bus;

class io
{
public:
	io();
	io(bus* b);
	void set_bus(bus* b);

	uint8_t io_read(uint16_t address);
	void io_write(uint16_t address, uint8_t value);

private:
	char serial_data[2];
	bus* b;

	//TODO: temp
	uint8_t ly = 0;
};

