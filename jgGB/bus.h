#pragma once
#include "common.h"
#include "cart.h"

class cpu;
class ram;
class io;
class timer;

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
	void set_io(io* i);
	void set_timer(timer* t);

	//bus passing messages between its members
	uint8_t bus_get_cpu_int_flags();
	void bus_set_cpu_int_flags(uint8_t value);
	uint8_t bus_timer_read(uint16_t  address);
	void bus_timer_write(uint16_t address, uint8_t value);
	void bus_timer_tick();

private:
	cart* crt;
	ram* r;
	cpu* c;
	io* i;
	timer* t;

};

