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

uint32_t ppu::fetch_sprite_pixels(int bit, uint32_t color, uint8_t bg_color)
{
	for (int i = 0; i < fetched_entry_count; i++)
	{
		int sp_x = (fetched_entries[i].x - 8) + (l->get_scroll_x() % 8);

		if (sp_x + 8 < fifo_x)
		{
			//past pixel point already
			continue;
		}

		int offset = fifo_x - sp_x;

		if (offset < 0 || offset > 7)
		{
			//out of bounds
			continue;
		}

		bit = (7 - offset);

		if (fetched_entries[i].f_x_flip)
		{
			bit = offset;
		}

		uint8_t hi = !!(fetch_entry_data[i * 2] & (1 << bit));
		uint8_t lo = !!(fetch_entry_data[(i * 2)+1] & (1 << bit)) << 1;

		bool bg_priority = fetched_entries[i].f_bgp;

		if (!(hi | lo))
		{
			//transparent
			continue;
		}

		if (!bg_priority || bg_color == 0)
		{
			color = (fetched_entries[i].f_pn) ? l->get_sp_colors(1, hi | lo) : l->get_sp_colors(0, hi | lo);

			if (hi | lo)
			{
				break;
			}
		}
	}

	return color;
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

		if (!(l->lcdc_bgw_enable()))
		{
			color = l->get_bg_colors(0);
		}

		if (l->lcdc_obj_enable())
		{
			color = fetch_sprite_pixels(bit, color, hi | lo);
		}

		if (x >= 0)
		{
			pixel_fifo.push(color);
			fifo_x++;
		}
	}
	return true;
}

void ppu::pipeline_load_sprite_tile()
{
	for (int i = 0; i < line_sprites.size(); i++)
	{
		int sp_x = (line_sprites[i].x - 8) + (l->get_scroll_x() % 8);

		if (((sp_x >= fetch_x) && (sp_x < fetch_x + 8)) || ((sp_x + 8 >= fetch_x) && (sp_x + 8 < fetch_x + 8)))
		{
			//need to add entry
			fetched_entries[fetched_entry_count++] = line_sprites[i];
		}

		if (fetched_entry_count >= 3)
		{
			//max checking 3 sprites on pixels
			break;
		}
	}
}

void ppu::pipeline_load_sprite_data(uint8_t offset)
{
	int cur_y = l->get_ly();
	uint8_t sprite_height = l->lcdc_obj_height();

	for (int i = 0; i < fetched_entry_count; i++)
	{
		uint8_t ty = ((cur_y + 16) - fetched_entries[i].y) * 2;

		if (fetched_entries[i].f_y_flip)
		{
			//flip upside down
			ty = ((sprite_height * 2) - 2) - ty;
		}
		uint8_t tile_index = fetched_entries[i].tile;

		if (sprite_height == 16)
		{
			tile_index &= ~(1); //remove last bit
		}

		fetch_entry_data[(i * 2) + offset] = b->bus_read(0x8000 + (tile_index * 16) + ty + offset);
	}
}

void ppu::pipeline_load_window_tile()
{
	if (!l->window_visible())
	{
		return;
	}
	uint8_t window_y = l->get_win_y();
	uint8_t window_x = l->get_win_x();

	if ((fetch_x + 7 >= window_x) && (fetch_x + 7 < window_x + YRES + 14))
	{
		if ((l->get_ly() >= window_y) && (l->get_ly() < window_y + XRES))
		{
			uint8_t w_tile_y = l->get_window_line() / 8;
			bgw_fetch_data[0] = b->bus_read(l->lcdc_win_map_area() + 
				((fetch_x + 7 - window_x) / 8) + (w_tile_y * 32));
			if (l->lcdc_bgw_data_area() == 0x8800)
			{
				bgw_fetch_data[0] += 128;
			}
		}
	}
}

void ppu::pipeline_fetch()
{
	switch (cur_fetch_state)
	{
		case FS_TILE:
		{
			fetched_entry_count = 0;
			if (l->lcdc_bgw_enable())
			{
				bgw_fetch_data[0] = b->bus_read(l->lcdc_bg_map_area() + (map_x / 8) + ((map_y / 8) * 32));

				if (l->lcdc_bgw_data_area() == 0x8800)
				{
					bgw_fetch_data[0] += 128;
				}

				pipeline_load_window_tile();
			}

			if (l->lcdc_obj_enable() && !line_sprites.empty())
			{
				pipeline_load_sprite_tile();
			}

			cur_fetch_state = FS_DATA0;
			fetch_x += 8;
		} break;

		case FS_DATA0:
		{
			bgw_fetch_data[1] = b->bus_read(l->lcdc_bgw_data_area() + (bgw_fetch_data[0] * 16) + tile_y);

			pipeline_load_sprite_data(0);

			cur_fetch_state = FS_DATA1;
		} break;

		case FS_DATA1:
		{
			bgw_fetch_data[2] = b->bus_read(l->lcdc_bgw_data_area() + (bgw_fetch_data[0] * 16) + tile_y + 1);

			pipeline_load_sprite_data(1);

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