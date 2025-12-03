#include "aChannel2.h"
#include "apu.h"

aChannel2::aChannel2(apu* a) : aChannelBase(a)
{
	initializing = true;
}

void aChannel2::triggerChannel()
{
	initializing = false;

	// check if DAC is enabled
	if (!dacOn())
	{
		return;  //TODO: is this correct?  Or should it only not enable channel
	}

	//Channel is enabled
	channelEnabled = true;

	//if length timer is expired, reset it
	if (timerCurrentLength >= 64)
	{
		timerCurrentLength = timerLength;
	}

	//set period divider to the contents of NR13 and NR14
	uint8_t lo = a->get_reg(2, 3);
	uint8_t hi = a->get_reg(2, 4);
	hi &= 0b111;  //just get bottom 3 bits
	periodCounter = (hi << 8) | lo;

	//Envelope timer is reset
	envIncrease = BIT(a->get_reg(2,2),3);
	envSweepPace = a->get_reg(2,2) & 0x7;  //bottom 3 bits
	envCounter = 0;

	
	//Volume is set to contents of NR22 initial volume
	curVolume = ((a->get_reg(2, 2)) & 0xF0) >> 4;
	
	//Sweep things

}



void aChannel2::tickPeriodCounter()
{

	if (periodCounter == 0x7FF)
	{
		if (!initializing)
		{
			dutyCounter = (dutyCounter + 1) % 8;
		}

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

int16_t aChannel2::generateSample()
{
	uint8_t dutyCycle = ((a->get_reg(2, 1) & 0xC0) >> 6);
	float sample = Wave[dutyCycle][dutyCounter];
	float modifier = ((float)curVolume) / 16.0;
	sample = sample * modifier;
	sample = std::round(sample);
	return dac((uint8_t)sample, (dacOn()) && (channelEnabled));

}


void aChannel2::channelAPUOff()
{
	dutyCounter = 0;
}

void aChannel2::envelopeSweepTick()
{
	if (envSweepPace == 0)  //sweep is turned off
	{
		return;
	}

	if (envCounter >= envSweepPace)
	{
		if (envIncrease)
		{
			if (curVolume < 0xF)
			{
				curVolume++;
			}
		}
		else
		{
			if (curVolume > 0)
			{
				curVolume--;
			}
		}

	}

	envCounter = (envCounter + 1) % (envSweepPace + 1);
}