#pragma once
#include "common.h"
class ram
{
public: 
	ram();

	uint8_t wram_read(uint16_t address);
	void wram_write(uint16_t address, uint8_t value);

	uint8_t hram_read(uint16_t address);
	void hram_write(uint16_t address, uint8_t value);

private:
	uint8_t wram[0x2000];
	uint8_t hram[0x80];

};

