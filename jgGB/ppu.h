#pragma once
#include "common.h"
#include <queue>
#include <vector>

//forward declare
class bus;
class lcd;
class ui;





class ppu
{
public:

	static const int LINES_PER_FRAME = 154;
	static const int TICKS_PER_LINE = 456;
	static const int YRES = 144;
	static const int XRES = 160;

	ppu(bus *b, lcd* l, ui* u);
	~ppu();
	void ppu_tick();

	void ppu_vram_write(uint16_t address, uint8_t value);
	uint8_t ppu_vram_read(uint16_t address);
	void ppu_oam_write(uint16_t address, uint8_t value);
	uint8_t ppu_oam_read(uint16_t address);

/*
 Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
 Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
 Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
 Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
 Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
 Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
 */


	typedef struct {
		uint8_t y;
		uint8_t x;
		uint8_t tile;

		uint8_t f_cgb_pn		: 3;
		uint8_t f_cgb_vram_bank : 1;
		uint8_t f_pn			: 1;
		uint8_t f_x_flip		: 1;
		uint8_t f_y_flip		: 1;
		uint8_t f_bgp			: 1;

	} oam_entry;

	uint32_t ppu_get_current_frame();

	typedef enum {
		FS_TILE,
		FS_DATA0,
		FS_DATA1,
		FS_IDLE,
		FS_PUSH
	} fetch_state;

	uint32_t* get_video_buffer();

private:
//	uint32_t* video_buffer;
	uint32_t video_buffer[YRES * XRES];
	uint8_t vram[0x2000];

	oam_entry oam_ram[40];
	uint32_t current_frame;
	uint32_t line_ticks;

	

	bus* b;
	lcd* l;
	ui* u;

	//frame timing variables

	const uint64_t target_frame_time = 1000 / 60;
	uint64_t prev_frame_time = 0;
	uint64_t start_timer = 0;
	uint64_t frame_count = 0;

	//state machine functions
	void ppu_mode_oam();
	void ppu_mode_xfer();
	void ppu_mode_vblank();
	void ppu_mode_hblank();

	//pixel fifo data
	std::queue<uint32_t> pixel_fifo;
	fetch_state cur_fetch_state;
	uint8_t line_x;
	uint8_t pushed_x;
	uint8_t fetch_x;
	uint8_t bgw_fetch_data[3];
	uint8_t fetch_entry_data[6];
	uint8_t map_y;
	uint8_t map_x;
	uint8_t tile_y;
	uint8_t fifo_x;

	//object fifo data
	uint8_t line_sprite_count;
	uint8_t fetched_entry_count;
	oam_entry fetched_entries[3];

	//pixel fifo functions
	uint32_t pipeline_fifo_pop();
	bool pipeline_fifo_add();
	void pipeline_fetch();
	void pipeline_push_pixel();
	void pipeline_process();
	void pipeline_fifo_reset();

	//object fifo functions
	std::vector<oam_entry> line_sprites;
	void load_line_sprites();
	uint32_t fetch_sprite_pixels(int bit, uint32_t color, uint8_t bg_color);
	void pipeline_load_sprite_tile();
	void pipeline_load_sprite_data(uint8_t offset);

	//window functions
	void pipeline_load_window_tile();
};

