#include "ppu.h"

ppu::ppu()
{
	//this->video_buffer = (uint32_t *) malloc(YRES * XRES * sizeof(uint32_t));
	this->video_buffer = new(uint32_t[YRES * XRES]);
}

ppu::~ppu()
{
	delete(video_buffer);
}

void ppu::ppu_init()
{

}

void ppu::ppu_vram_write(uint16_t address, uint8_t value)
{
	vram[address - 0x8000] = value;
}
uint8_t ppu::ppu_vram_read(uint16_t address)
{
	return vram[address - 0x8000];
}

void ppu::ppu_oam_write(uint16_t address, uint8_t value)
{
	if (address >= 0xFE00)
	{
		address -= 0xFE00;
	}
	oam_ram[address] = value;
}
uint8_t ppu::ppu_oam_read(uint16_t address)
{
	if (address >= 0xFE00)
	{
		address -= 0xFE00;
	}

	return oam_ram[address];
}