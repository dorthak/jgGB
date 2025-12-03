#include "aChannelBase.h"
#include "apu.h"

aChannelBase::aChannelBase(apu* a)
{
	this->a = a;
	channelEnabled = false;
}

aChannelBase::~aChannelBase()
{

}

void aChannelBase::tickPeriodCounter()
{
	return;
}


void aChannelBase::lengthEnable()
{
	lengthEnabled = true;
	timerCurrentLength = timerLength;
}

bool aChannelBase::dacOn()
{
	return (a->get_reg(2, 2) & 0xF8);
}

void aChannelBase::lengthTick()
{
	timerCurrentLength++;

	if (timerCurrentLength >= 64)
	{
		channelEnabled = false;

	}
}

void aChannelBase::channelAPUOff()
{

}

void aChannelBase::envelopeSweepTick()
{

}

int16_t aChannelBase::dac(uint8_t input, bool dac_on)  
{
	if (input > 0xF)
	{
		std::cerr << "Invalid digital audio value, should be between 0x0 and 0xF.  Recieved: " << input << std::endl;
	}

	int16_t value = input * 0x11;  //rescale from 0-F to 0-FF
	value = value - (0xFF / 2); //shift center point down to 0 for signed value
	if (dac_on)
	{
		lastDac = value;
		return value;
	}
	else  //TODO:  Very simplified decay
	{
		lastDac = lastDac / 2;
		return lastDac;
	}
}