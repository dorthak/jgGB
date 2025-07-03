#pragma once
#include "common.h"

class apu;

class aChannelBase {
public:
	aChannelBase(apu* a);
	~aChannelBase();
	virtual void trigger_channel() = 0;
protected:
	apu* a;

};