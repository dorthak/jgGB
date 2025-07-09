#pragma once
#include "common.h"

class apu;

class aChannelBase {
public:
	aChannelBase(apu* a);
	~aChannelBase();
	virtual void trigger_channel() = 0;
	virtual void tickPeriodCounter();
	virtual uint8_t generateSample() = 0;
protected:
	apu* a;
	bool dacOn = false;
	uint16_t periodCounter = 0;


};