#pragma once
#include "common.h"
#include "cart.h"

class cpu;
class ram;

class bus
{
public:

	bus();
	uint8_t bus_read(uint16_t address);
	void bus_write(uint16_t address, uint8_t value);
	void bus_write16(uint16_t address, uint16_t value);
	uint16_t bus_read16(uint16_t address);
	void set_cart(cart* crt);
	void set_ram(ram* r);
	void set_cpu(cpu* c);

private:
	cart* crt;
	ram* r;
	cpu* c;

};

