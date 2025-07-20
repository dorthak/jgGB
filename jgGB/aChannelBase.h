#pragma once
#include "common.h"

class apu;

class aChannelBase {
public:
	aChannelBase(apu* a);
	~aChannelBase();
	virtual void triggerChannel() = 0;
	virtual void tickPeriodCounter();
	virtual int16_t generateSample() = 0;
	virtual void lengthEnable();
	virtual void lengthTick();
	virtual void channelAPUOff();
	virtual void envelopeSweepTick();

	bool lengthEnabled;
	uint8_t timerLength = 0;

	bool envelopeEnabled;

	bool channelEnabled;

protected:
	apu* a;

	uint16_t periodCounter = 0;

	uint8_t timerCurrentLength = 0;

	bool dacOn();

	uint8_t curVolume = 0;

	int16_t dac(uint8_t input, bool dac_on);

	int16_t lastDac = 0;

};