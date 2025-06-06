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
		case 0xFF01: return serial_data[0]; 
		case 0xFF02: return serial_data[1]; 
		case 0xFF04:
		case 0xFF05:
		case 0xFF06:
		case 0xFF07: 
			return b->bus_timer_read(address); 
		case 0xFF0F: return b->bus_get_cpu_int_flags(); 

//		case 0xFF44: return 0x90;  //TODO: temp, just here to make 01-special go
		case 0xFF44: return ly++;  //TODO: temp, just here to make tetris work
		default:
			//printf("UNSUPPORTED bus_read(%04X)\n", address);
			return 0;

	}
}
void io::io_write(uint16_t address, uint8_t value)
{
	switch (address)
	{
		case 0xFF01: serial_data[0] = value; break;
		case 0xFF02: serial_data[1] = value; break;
		case 0xFF04:
		case 0xFF05:
		case 0xFF06:
		case 0xFF07: 
			b->bus_timer_write(address, value); break;
		case 0xFF0F: b->bus_set_cpu_int_flags(value); break;
		

		default:
			//printf("UNSUPPORTED bus_write(%04X)\n", address);
			return;
	}
}
