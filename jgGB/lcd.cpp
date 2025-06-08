#include "lcd.h"
#include "bus.h"


lcd::lcd(bus* b)
{
	this->b = b;
	
	lcdc = 0x91;
	scroll_x = 0;
	scroll_y = 0;
	ly = 0;
	ly_compare = 0;
	bg_palette = 0xFF;
	obj_palette[0] = 0xFF;
	obj_palette[1] = 0xFF;
	win_y = 0;
	win_x = 0;
	
	for (int i = 0; i < 4; i++)
	{
		bg_colors[i] = colors_default[i];
		sp1_colors[i] = colors_default[i];
		sp2_colors[i] = colors_default[i];
	}

	//scroll_y = 0xBC;
	//lcd_write(0xFF45, 0x12);
	//uint8_t temp = lcd_read(0xFF42);
	//temp = lcd_read(0xFF45);
	//lcd_write(0xFF46, 0x25);
	//std::cout << temp << std::endl;

}

lcd::~lcd()
{

}
bool lcd::lcdc_bgw_enable()
{
	return BIT(lcdc, 0);
}
bool lcd::lcdc_obj_enable()
{
	return BIT(lcdc, 1);
}
uint8_t lcd::lcdc_obj_height()
{
	bool bit = BIT(lcdc, 2);
	return bit ? 16 : 8;
}
uint16_t lcd::lcdc_bg_map_area()
{
	bool bit = BIT(lcdc, 3);
	return bit ? 0x9C00 : 0x9800;
}
uint16_t lcd::lcdc_bg_data_area()
{
	bool bit = BIT(lcdc, 4);
	return bit ? 0x8000 : 0x8800;
}
bool lcd::lcdc_win_enable()
{
	return BIT(lcdc, 5);
}
uint16_t lcd::lcdc_win_map_area()
{
	bool bit = BIT(lcdc, 6);
	return bit ? 0x9C00 : 0x9800;
}
bool lcd::lcdc_lcd_enable()
{
	return BIT(lcdc, 7);
}

lcd::lcd_mode lcd::lcds_mode()
{
	return (lcd_mode) (lcds & 0b11);
}
void lcd::lcds_mode_set(lcd_mode mode)
{
	lcds = lcds & ~0b11;
	lcds = lcds | mode;
}

bool lcd::lcds_lyc()
{
	return BIT(lcds, 2);
}
void lcd::lcds_lyc_set(bool b)
{
	BIT_SET(lcds, 2, b);
}

bool lcd::lcds_stat_int(stat_src src)
{
	return lcds & src;
}

uint8_t lcd::lcd_read(uint16_t address)
{
	uint8_t offset = (address - 0xFF40);
	uint8_t* p = (uint8_t*)this;

	return p[offset];
}

void lcd::update_palette(uint8_t palette_data, uint8_t pal)
{
	uint32_t* p_colors = bg_colors;

	switch (pal)
	{
		case 1:
			p_colors = sp1_colors;
			break;
		case 2:
			p_colors = sp2_colors;
			break;
	}

	p_colors[0] = colors_default[palette_data & 0b11];
	p_colors[1] = colors_default[(palette_data >> 2) & 0b11];
	p_colors[2] = colors_default[(palette_data >> 4) & 0b11];
	p_colors[3] = colors_default[(palette_data >> 6) & 0b11];
}

void lcd::lcd_write(uint16_t address, uint8_t value)
{
	uint8_t offset = (address - 0xFF40);
	uint8_t* p = (uint8_t*)this;
	p[offset] = value;

	switch (address) {
		case 0xFF46: //DMA
			b->dma_start(value);
			break;
		case 0xFF47:  //BGP - BG palette data
			update_palette(value, 0);
			break;
		case 0xFF48: //OBP0 - ObJ palette 0 data
			update_palette(value & (~0b11), 1);
			break;
		case 0xFF49: //OBP1 - ObJ palette 1 data
			update_palette(value & (~0b11), 2);
			break;
	}
}

void lcd::increment_ly()
{
	ly++;

	if (ly == ly_compare)
	{
		lcds_lyc_set(true);
		if (lcds_stat_int(SS_LYC))
		{
			b->bus_request_cpu_interrupt(instdata::IT_LCD_STAT);
		}
	} else {
		lcds_lyc_set(false);
	}
}

void lcd::set_ly(uint8_t val)
{
	ly = val;
}
uint8_t lcd::get_ly()
{
	return ly;
}