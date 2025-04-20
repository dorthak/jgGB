#include "timer.h"

timer::timer()
{

}
timer::timer(bus* b)
{
	this->b = b;
}
void timer::set_bus(bus* b)
{
	this->b = b;
}

void timer::timer_init()
{

}
void timer::timer_tick()
{

}

uint8_t timer::timer_read(uint16_t  address)
{
	return 0;
}

void timer::timer_write(uint16_t address, uint8_t value)
{

}