#include "io.h"
#include "bus.h"

io::io()
{

}
io::io(bus* b)
{
	this->b = b;
}
void io::set_bus(bus* b)
{
	this->b = b;
}

uint8_t io::io_read(uint16_t address)
{
	switch(address)
	{
		case 0xFF00: return gamepad_get_output();
		case 0xFF01: return serial_data[0]; 
		case 0xFF02: return serial_data[1]; 
		case 0xFF04:
		case 0xFF05:
		case 0xFF06:
		case 0xFF07: 
			return b->bus_timer_read(address); 
		case 0xFF0F: return b->bus_get_cpu_int_flags(); 

		default:
			//printf("UNSUPPORTED bus_read(%04X)\n", address);
			return 0;
	}
}
void io::io_write(uint16_t address, uint8_t value)
{
	switch (address)
	{
		case 0xFF00: gamepad_set_sel(value); break;
		case 0xFF01: serial_data[0] = value; break;
		case 0xFF02: serial_data[1] = value; break;
		case 0xFF04:
		case 0xFF05:
		case 0xFF06:
		case 0xFF07: b->bus_timer_write(address, value); break;
		case 0xFF0F: b->bus_set_cpu_int_flags(value); break;
		

		default:
			//printf("UNSUPPORTED bus_write(%04X)\n", address);
			return;
	}
}


uint8_t io::gamepad_get_output()
{
	uint8_t output = 0xCF;
	if (button_sel)
	{
		if (gamepad.start)
		{
			output &= ~(1 << 3);
		}
		if (gamepad.select) {
			output &= ~(1 << 2);
		}
		if (gamepad.a) {
			output &= ~(1 << 0);
		}
		if (gamepad.b) {
			output &= ~(1 << 1);
		}
	}

	if (dir_sel)
	{
		if (gamepad.left)
		{
			output &= ~(1 << 1);
		}
		if (gamepad.right) {
			output &= ~(1 << 0);
		}
		if (gamepad.up) {
			output &= ~(1 << 2);
		}
		if (gamepad.down) {
			output &= ~(1 << 3);
		}
	}
	return output;
}
bool io::gamepad_button_sel()
{
	return button_sel;
}
bool io::gamepad_dir_sel()
{
	return dir_sel;
}
void io::gamepad_set_sel(uint8_t value)
{
	button_sel = !(value & 0x20);
	dir_sel = !(value & 0x10);
}
