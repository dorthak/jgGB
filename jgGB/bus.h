#pragma once
#include "common.h"
#include "cart.h"


class bus
{
public:

	bus();
	uint8_t bus_read(uint16_t address);
	void bus_write(uint16_t address, uint8_t value);
	void set_cart(cart* crt);

private:
	cart* crt;
};

