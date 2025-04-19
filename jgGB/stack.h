#pragma once
#include "common.h"
#include "bus.h"

class cpu;

class stack
{
public:
	stack(bus* b);

	void set_cpu(cpu* c);

	void stack_push(uint8_t data);
	void stack_push16(uint16_t data);

	uint8_t stack_pop();
	uint16_t stack_pop16();

private:
	bus* b;
	cpu* c;
};

