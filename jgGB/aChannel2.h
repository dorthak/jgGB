#pragma once
#include "aChannelBase.h"

class aChannel2 :
    public aChannelBase
{
public:
    aChannel2(apu* a);
    void triggerChannel() override;
    void tickPeriodCounter() override;
    int16_t generateSample() override;
    void channelAPUOff() override;
    void envelopeSweepTick() override;

protected:
    uint8_t dutyCounter = 0;
    
    //envelope
    bool envIncrease;
    uint8_t envSweepPace;
    uint8_t envCounter = 0;


    bool initializing;

    uint8_t Wave[4][8] = {
        { 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0x0 },
        { 0x0, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0x0 },
        { 0x0, 0xF, 0xF, 0xF, 0xF, 0x0, 0x0, 0x0 },
        { 0xF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF }
    };

    
  

};

