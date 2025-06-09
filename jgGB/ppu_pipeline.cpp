#include "ppu.h"
#include "bus.h"
#include "lcd.h"
#include "ui.h"

uint32_t ppu::pipeline_fifo_pop()
{
	uint32_t temp = pixel_fifo.front();
	pixel_fifo.pop();
	return temp;
}

bool ppu::pipeline_fifo_add()
{
	if (pixel_fifo.size() > 8)
	{
		//fifo is full
		return false;
	}

	int x = fetch_x - (8 - (l->get_scroll_x() % 8));

	for (int bit = 7; bit >=0; bit--)
	{
		uint8_t hi = !!(bgw_fetch_data[1] & (1 << bit));
		uint8_t lo = !!(bgw_fetch_data[2] & (1 << bit)) << 1;
		uint32_t color = l->get_bg_colors(hi | lo);

		if (x >= 0)
		{
			pixel_fifo.push(color);
			fifo_x++;
		}
	}
	return true;
}

void ppu::pipeline_fetch()
{
	switch (cur_fetch_state)
	{
		case FS_TILE:
		{
			if (l->lcdc_bgw_enable())
			{
				bgw_fetch_data[0] = b->bus_read(l->lcdc_bg_map_area() + (map_x / 8) + ((map_y / 8) * 32));
				if (l->lcdc_bgw_data_area() == 0x8800)
				{
					bgw_fetch_data[0] += 128;
				}
			}
			cur_fetch_state = FS_DATA0;
			fetch_x += 8;
		} break;

		case FS_DATA0:
		{
			bgw_fetch_data[1] = b->bus_read(l->lcdc_bgw_data_area() + (bgw_fetch_data[0] * 16) + tile_y);
			cur_fetch_state = FS_DATA1;
		} break;

		case FS_DATA1:
		{
			bgw_fetch_data[2] = b->bus_read(l->lcdc_bgw_data_area() + (bgw_fetch_data[0] * 16) + tile_y + 1);
			cur_fetch_state = FS_IDLE;
		} break;

		case FS_IDLE:
		{
			cur_fetch_state = FS_PUSH;
		} break;

		case FS_PUSH:
		{
			if (pipeline_fifo_add())
			{
				cur_fetch_state = FS_TILE;
			}
		} break;
	}
}

void ppu::pipeline_push_pixel()
{
	if (pixel_fifo.size() > 8)
	{
		uint32_t pixel_data = pipeline_fifo_pop();

		if (line_x >= (l->get_scroll_x() % 8))
		{
			video_buffer[pushed_x + (l->get_ly() * XRES)] = pixel_data;
			pushed_x++;
		}
		line_x++;
	}
}

void ppu::pipeline_process()
{
	map_y = l->get_ly() + l->get_scroll_y();
	map_x = fetch_x + l->get_scroll_x();
	tile_y = ((l->get_ly() + l->get_scroll_y()) % 8) * 2;

	if (!(line_ticks & 1))
	{
		pipeline_fetch();
	}
	pipeline_push_pixel();
}

void ppu::pipeline_fifo_reset()
{
	while (!(pixel_fifo.empty()))
	{
		pixel_fifo.pop();
	}
}