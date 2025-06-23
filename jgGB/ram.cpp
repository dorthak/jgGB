#include "ram.h"

ram::ram()
{

}

uint8_t ram::wram_read(uint16_t address)
{
	address -= 0xC000;

	if (address >= 0x2000)
	{
		printf("INVALID WRAM ADDR %08X\n", address + 0xC000);
		exit(-1);
	}
	return wram[address];
}
void ram::wram_write(uint16_t address, uint8_t value)
{
	address -= 0xC000;

	if (address >= 0x2000)
	{
		printf("INVALID WRAM ADDR %08X\n", address + 0xC000);
		exit(-1);
	}
	wram[address] = value;
}

uint8_t ram::hram_read(uint16_t address)
{
	address -= 0xFF80;

	if (address >= 0x80)
	{
		printf("INVALID HRAM ADDR %08X\n", address + 0xFF80);
		exit(-1);
	}
	return hram[address];
}
void ram::hram_write(uint16_t address, uint8_t value)
{
	address -= 0xFF80;

	if (address >= 0x80)
	{
		printf("INVALID HRAM ADDR %08X\n", address + 0xFF80);
		exit(-1);
	}
	hram[address] = value;
}
