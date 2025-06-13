#pragma once
#include "common.h"

class bus;

class io
{
public:
	io();
	io(bus* b);
	void set_bus(bus* b);

	uint8_t io_read(uint16_t address);
	void io_write(uint16_t address, uint8_t value);

	typedef struct {
		bool start;
		bool select;
		bool a;
		bool b;
		bool up;
		bool down;
		bool left;
		bool right;
	} gamepad_state;
	
	uint8_t gamepad_get_output();
	bool gamepad_button_sel();
	bool gamepad_dir_sel();
	void gamepad_set_sel(uint8_t value);

	gamepad_state gamepad;

private:
	char serial_data[2];
	bus* b;

	//gamepad stuff
	
	bool button_sel;
	bool dir_sel;
	
};

