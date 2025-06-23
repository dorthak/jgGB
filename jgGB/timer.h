#pragma once
#include "common.h"

class bus;

class timer
{
public:
	timer();
	timer(bus* b);
	void set_bus(bus* b);

	void timer_tick();
	uint8_t timer_read(uint16_t  address);
	void timer_write(uint16_t address, uint8_t value);

private:
	bus* b;
	uint16_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;
};

