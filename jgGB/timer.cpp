#include "timer.h"
#include "bus.h"

timer::timer()
{
	div = 0xAC00;
}
timer::timer(bus* b)
{
	this->b = b;
	div = 0xAC00;
}
void timer::set_bus(bus* b)
{
	this->b = b;
}

void timer::timer_tick()
{
	uint16_t prev_div = div;
	
	div++;

	bool timer_update = false;

	switch (tac & 0b11)
	{
		case 0b00:
			timer_update = (prev_div & (1 << 9)) && (!(div & (1 << 9)));
			break;
		case 0b01:
			timer_update = (prev_div & (1 << 3)) && (!(div & (1 << 3)));
			break;
		case 0b10:
			timer_update = (prev_div & (1 << 5)) && (!(div & (1 << 5)));
			break;
		case 0b11:
			timer_update = (prev_div & (1 << 7)) && (!(div & (1 << 7)));
			break;
	}

	if (timer_update && (tac & (1 << 2)))
	{
		tima++;

		if (tima == 0xFF)
		{
			tima = tma;
			b->bus_request_cpu_interrupt(instdata::IT_TIMER);
		}
	}
}

uint8_t timer::timer_read(uint16_t  address)
{
	switch (address) {
		case 0xFF04:
			return div >> 8;
		case 0xFF05:
			return tima;
		case 0xFF06:
			return tma;
		case 0xFF07:
			return tac;
	}
	return 0;
}

void timer::timer_write(uint16_t address, uint8_t value)
{
	switch (address) {
		case 0xFF04:
			//DIV
			div = 0;
			break;

		case 0xFF05:
			//TIMA
			tima = value;
			break;

		case 0xFF06:
			//TMA
			tma = value;
			break;

		case 0xFF07:
			//TAC
			tac = value;
			break;
	}
}