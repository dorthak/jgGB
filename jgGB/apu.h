#pragma once
#include "common.h"


class bus;
class ui;
class aChannelBase;

class apu
{
public:
	apu(bus* b, ui* u);
	~apu();

	void apu_write(uint16_t address, uint8_t value);
	uint8_t apu_read(uint16_t address);

	uint8_t get_reg(uint8_t channel, uint8_t value);

private:
	struct {
		uint8_t NR10;
		uint8_t NR11;
		uint8_t NR12;
		uint8_t NR13;
		uint8_t NR14;
		uint8_t NR21;
		uint8_t NR22;
		uint8_t NR23;
		uint8_t NR24;
		uint8_t NR30;
		uint8_t NR31;
		uint8_t NR32;
		uint8_t NR33;
		uint8_t NR34;
		uint8_t NR41;
		uint8_t NR42;
		uint8_t NR43;
		uint8_t NR44;
		uint8_t NR50;
		uint8_t NR51;
		uint8_t NR52;
	} regs;

	uint8_t WaveRAM[16];

	bus* b;
	ui* u;

	aChannelBase* channels[4];

	uint8_t* reg_array[5][5]; //channel, register

};

