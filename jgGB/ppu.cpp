#include "ppu.h"
#include "bus.h"
#include "lcd.h"

ppu::ppu(bus *b, lcd* l)
{
	this->b = b;
	this->l = l;

	current_frame = 0;
	line_ticks = 0;

	//this->video_buffer = (uint32_t *) malloc(YRES * XRES * sizeof(uint32_t));
	//this->video_buffer = new(uint32_t[YRES * XRES]);

	//std::cout << "oam_entry size: " << sizeof(oam_entry) << ".  oam_ram size :" << sizeof(oam_ram) << " bytes.  oam_ram size :" << sizeof(oam_ram) / sizeof(oam_ram[0]) << " entries." << std::endl;

	l->lcds_mode_set(lcd::MODE_OAM);

	memset(oam_ram, 0, sizeof(oam_ram));
	memset(video_buffer, 0, YRES * XRES * sizeof(uint32_t));

}

ppu::~ppu()
{
	//delete(video_buffer);
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
	uint8_t* p = (uint8_t*)oam_ram;
	p[address] = value;
}
uint8_t ppu::ppu_oam_read(uint16_t address)
{
	if (address >= 0xFE00)
	{
		address -= 0xFE00;
	}
	uint8_t* p = (uint8_t*)oam_ram;
	return p[address];
}