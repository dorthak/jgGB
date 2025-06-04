#pragma once
#include "common.h"

static const int LINES_PER_FRAME = 154;
static const int TICKS_PER_LINE = 456;
static const int YRES = 144;
static const int XRES = 160;

class ppu
{
public:
	ppu();
	~ppu();
	void ppu_init();
	void ppu_tick();

	void ppu_vram_write(uint16_t address, uint8_t value);
	uint8_t ppu_vram_read(uint16_t address);
	void ppu_oam_write(uint16_t address, uint8_t value);
	uint8_t ppu_oam_read(uint16_t address);

private:
	uint32_t* video_buffer;
	uint8_t vram[0x2000];

	//TODO: This is temporary
	uint8_t oam_ram[0xA0];
};

