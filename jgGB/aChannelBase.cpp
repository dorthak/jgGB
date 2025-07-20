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