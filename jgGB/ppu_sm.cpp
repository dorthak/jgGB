#include "ppu.h"
#include "bus.h"
#include "lcd.h"
#include "ui.h"

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

		}
		else {
			l->lcds_mode_set(lcd::MODE_OAM);
		}
		line_ticks = 0;
	}

}
