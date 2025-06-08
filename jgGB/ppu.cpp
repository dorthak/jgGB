#include "ppu.h"
#include "bus.h"
#include "lcd.h"
#include "ui.h"

ppu::ppu(bus *b, lcd* l, ui* u)
{
	this->b = b;
	this->l = l;
	this->u = u;

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

void ppu::ppu_tick()
{
	line_ticks++;
	switch (l->lcds_mode()) {
		case lcd::MODE_OAM:
			ppu_mode_oam();
			break;
		case lcd::MODE_XFER:
			ppu_mode_xfer();
			break;
		case lcd::MODE_VBLANK:
			ppu_mode_vblank();
			break;
		case lcd::MODE_HBLANK:
			ppu_mode_hblank();
			break;
	}
}


void ppu::ppu_mode_oam()
{
	if (line_ticks >= 80)
	{
		l->lcds_mode_set(lcd::MODE_XFER);
	}
}
void ppu::ppu_mode_xfer()
{
	if (line_ticks >= 80 + 172)
	{
		l->lcds_mode_set(lcd::MODE_HBLANK);
	}
}
void ppu::ppu_mode_vblank()
{
	if (line_ticks >= TICKS_PER_LINE)
	{
		l->increment_ly();

		if (l->get_ly() >= LINES_PER_FRAME)
		{
			l->lcds_mode_set(lcd::MODE_OAM);
			l->set_ly(0);
		}
		line_ticks = 0;
	}
}
void ppu::ppu_mode_hblank()
{
	if (line_ticks >= TICKS_PER_LINE)
	{
		l->increment_ly();

		if (l->get_ly() >= YRES)
		{
			l->lcds_mode_set(lcd::MODE_VBLANK);

			b->bus_request_cpu_interrupt(instdata::IT_VBLANK);

			if (l->lcds_stat_int(lcd::SS_VBLANK))
			{
				b->bus_request_cpu_interrupt(instdata::IT_LCD_STAT);
			}

			current_frame++;

			//calc FPS
			uint32_t end = u->get_ticks();
			uint32_t frame_time = end - prev_frame_time;

			if (frame_time < target_frame_time)
			{
				u->delay((target_frame_time - frame_time));
			}

			if (end - start_timer >= 1000)
			{
				uint32_t fps = frame_count;
				start_timer = end;
				frame_count = 0;

				std::cout << "FPS: " << fps << std::endl;
			}
			frame_count++;
			prev_frame_time = u->get_ticks();

		} else {
			l->lcds_mode_set(lcd::MODE_OAM);
		}
		line_ticks = 0;
	}

}

uint32_t ppu::ppu_get_current_frame()
{
	return current_frame;
}