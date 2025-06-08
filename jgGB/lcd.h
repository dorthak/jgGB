#pragma once
#include "common.h"

//forward declare
class bus;

class lcd
{
public:

	lcd(bus* b);
	~lcd();

	typedef enum
	{
		MODE_HBLANK,
		MODE_VBLANK,
		MODE_OAM,
		MODE_XFER,
	} lcd_mode;

	typedef enum
	{
		SS_HBLANK = (1 << 3),
		SS_VBLANK = (1 << 4),
		SS_OAM = (1 << 5),
		SS_LYC = (1 << 6),
	} stat_src;

	bool lcdc_bgw_enable();
	bool lcdc_obj_enable();
	uint8_t lcdc_obj_height();
	uint16_t lcdc_bg_map_area();
	uint16_t lcdc_bg_data_area();
	bool lcdc_win_enable();
	uint16_t lcdc_win_map_area();
	bool lcdc_lcd_enable();

	lcd_mode lcds_mode();
	void lcds_mode_set(lcd_mode mode);

	bool lcds_lyc();
	void lcds_lyc_set(bool b);

	bool lcds_stat_int(stat_src src);

	uint8_t lcd_read(uint16_t address);
	void lcd_write(uint16_t address, uint8_t value);

	void increment_ly();
	void set_ly(uint8_t val);
	uint8_t get_ly();

private:
	
	//registers
	uint8_t lcdc;
	uint8_t lcds;
	uint8_t scroll_y;
	uint8_t scroll_x;
	uint8_t ly;
	uint8_t ly_compare;
	uint8_t dma;
	uint8_t bg_palette;
	uint8_t obj_palette[2];
	uint8_t win_y;
	uint8_t win_x;

	uint32_t bg_colors[4];
	uint32_t sp1_colors[4];
	uint32_t sp2_colors[4];

	const unsigned long colors_default[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };
	bus* b;


	void update_palette(uint8_t palette_data, uint8_t pal);


};

