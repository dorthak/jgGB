#include "aChannel2.h"
#include "apu.h"

aChannel2::aChannel2(apu* a) : aChannelBase(a)
{

}

void aChannel2::trigger_channel()
{

}

void aChannel2::tickPeriodCounter()
{

	if (periodCounter == 0x7FF)
	{
		dutyCounter = (dutyCounter + 1) % 8;

		uint8_t lo = a->get_reg(2, 3);
		uint8_t hi = a->get_reg(2, 4);

		hi &= 0b111;  //just get bottom 3 bits
		periodCounter = (hi << 8) | lo;

	}
	else
	{
		periodCounter++;
	}
}

uint8_t aChannel2::generateSample()
{
	uint8_t dutyCycle = ((a->get_reg(2, 1) & 0xC0) >> 6);

	return Wave[dutyCycle][dutyCounter];
}