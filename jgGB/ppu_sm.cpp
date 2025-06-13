#include "bus.h"
#include "lcd.h"
#include "ppu.h"
#include "ui.h"
#include <algorithm>

void ppu::ppu_mode_oam()
{
	if (line_ticks >= 80)
	{
		l->lcds_mode_set(lcd::MODE_XFER);
		cur_fetch_state = FS_TILE;
		line_x = 0;
		fetch_x = 0;
		pushed_x = 0;
		fifo_x = 0;
	}

	if (line_ticks == 1)
	{
		//read oam on the first tick only
		line_sprite_count = 0;
		load_line_sprites();

	}
}
void ppu::ppu_mode_xfer()
{
	pipeline_process();

	if (pushed_x >= XRES)
	{
		pipeline_fifo_reset();
		l->lcds_mode_set(lcd::MODE_HBLANK);
		
		if (l->lcds_stat_int(lcd::SS_HBLANK))
		{
			b->bus_request_cpu_interrupt(instdata::IT_LCD_STAT);
		}
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
			l->set_window_line(0);
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

				//TODO: ?
				//trigger cart saving from here:  
				if (b->bus_cart_need_save())
				{
					b->bus_cart_battery_save();
				}
			}
			frame_count++;
			prev_frame_time = u->get_ticks();

		}
		else {
			l->lcds_mode_set(lcd::MODE_OAM);
		}
		line_ticks = 0;
	}

}

bool comp_oam(ppu::oam_entry a, ppu::oam_entry b)
{
	return a.x < b.x;
}

void ppu::load_line_sprites()
{
	int cur_y = l->get_ly();

	uint8_t sprite_height = l->lcdc_obj_height();

	line_sprites.clear();

	for (int i = 0; i < 40; i++)
	{
		oam_entry e = oam_ram[i];

		if (!e.x)
		{
			//x = 0 means not visible
			continue;
		}

		if (line_sprite_count >= 10)
		{
			//max 10 sprites per line
			break;
		}

		if ((e.y <= cur_y + 16) && (e.y + sprite_height > cur_y + 16))
		{
			// this sprite is on the current line
			line_sprites.push_back(e);
			line_sprite_count++;

		}
	}
	std::sort(line_sprites.begin(), line_sprites.end(), comp_oam);

}