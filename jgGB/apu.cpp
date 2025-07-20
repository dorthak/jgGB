#include "apu.h"
#include "bus.h"
#include "ui.h"
#include "audioUI.h"
#include "aChannelBase.h"
#include "aChannel2.h"


apu::apu(bus* b, ui* u, audioUI* aui)
{
	this->b = b;
	this->u = u;
	this->aui = aui;
	
	//init channels
	for (int i = 0;i < 4;i++)
	{
		channels[i] = nullptr;
	}
	aChannel2 *ch2 = new aChannel2(this);
	channels[1] = ch2;
	
	

	//initialize reg_array
	//channel, register
	//reg_array[4][5]; 
	for (int i = 0;i < 5;i++)
	{
		for (int j = 0;j < 5;j++)
		{
			reg_array[i][j] = nullptr;
		}
	}
	reg_array[0][0] = &(regs.NR50);
	reg_array[0][1] = &(regs.NR51);
	reg_array[0][1] = &(regs.NR52);
	
	reg_array[1][0] = &(regs.NR10);
	reg_array[1][1] = &(regs.NR11);
	reg_array[1][2] = &(regs.NR12);
	reg_array[1][3] = &(regs.NR13);
	reg_array[1][4] = &(regs.NR14);

	reg_array[2][1] = &(regs.NR21);
	reg_array[2][2] = &(regs.NR22);
	reg_array[2][3] = &(regs.NR23);
	reg_array[2][4] = &(regs.NR24);
		
	reg_array[3][0] = &(regs.NR30);
	reg_array[3][1] = &(regs.NR31);
	reg_array[3][2] = &(regs.NR32);
	reg_array[3][3] = &(regs.NR33);
	reg_array[3][4] = &(regs.NR34);

	reg_array[4][1] = &(regs.NR41);
	reg_array[4][2] = &(regs.NR42);
	reg_array[4][3] = &(regs.NR43);
	reg_array[4][4] = &(regs.NR44);

	div_apu = 0;
	//prev_div = b->bus_read(0xFF04); //read timer's DIV value
	prev_div = 0;

	apuOn = false;
}

apu::~apu ()
{
	for (int i = 0;i < 4;i++)
	{
		if (channels[i] != nullptr)
		{
			delete channels[i];
		}
	}
}


void apu::apu_write(uint16_t address, uint8_t value)
{

	if (address < 0xFF30)  //register
	{
		if ((!apuOn) && (!(address == 0xFF26)))
		{
			return;
		}
		uint8_t offset = (address - 0xFF10);
		uint8_t* p = (uint8_t*)&regs;
		p[offset] = value;

		switch (address) {
			case 0xFF16:  //NR21
			{
				channels[1]->timerLength = value & 0x3F; //set the timerLength from bit 0-5
			} break;

			
			case 0xFF19:  //NR24
			{
				if (BIT(value, 7))
				{
					channels[1]->triggerChannel();
				}
				if (BIT(value, 6))
				{
					channels[1]->lengthEnable();
				}
				else 
				{
					channels[1]->lengthEnabled = false;
				}
				

			} break;
			case 0xFF26:  //NR52
			{
				if (BIT(value, 7) == 1)
				{
					apu_on();
				}
				else
				{
					apu_off();
				}
			}

		}
			
	}
	else   //Wave Ram
	{
		WaveRAM[address - 0xFF30] = value;
	}


}
uint8_t apu::apu_read(uint16_t address)
{
	if (address < 0xFF30)  //register
	{

		uint8_t offset = (address - 0xFF10);
		uint8_t* p = (uint8_t*)&regs;

		if (address == 0xFF26) //Audio Master Control
		{
			uint8_t ch4 = 0 << 3;
			uint8_t ch3 = 0 << 2;
			uint8_t ch2 = (channels[1]->channelEnabled) << 1;
			uint8_t ch1 = 0 << 0;
			uint8_t mast = BIT(get_reg(0, 2), 7) << 7;

			uint8_t output = 0;
			output = mast | ch4 | ch3 | ch2 | ch1;

			p[offset] = output;
		}

		return p[offset];
	}
	else  //Wave Ram
	{
		return WaveRAM[address - 0xFF30];
	}

}

uint8_t apu::get_reg(uint8_t channel, uint8_t value)
{
	uint8_t* temp = reg_array[channel][value];
	if (temp != nullptr)
	{
		return *temp;
	}
	std::cerr << "Error: invalid audio register requested!" << std::endl;
	return 0xff;
}

void apu::apu_tick()
{
	uint16_t div = b->bus_read(0xFF04); //read timer's DIV value
	
	// div_apu logic
	if ((BIT(prev_div, 4) == 1) && (BIT(div, 4) == 0))
	{
		div_apu_tick();
		
		//TODO: Temporary
		//int8_t outputSamples[16];
		//for (int i = 0; i < 16; i++)
		//{
		//	outputSamples[i] = i;
		//}
		//aui->putAudio(outputSamples, 16);

	} 

	if (!apuOn)
	{
		return;
	}

	//period divider logic - every 4 ticks
	if ((div % 4) == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (channels[i] != nullptr)
			{
				channels[i]->tickPeriodCounter();
								
			}
		}
	}

	//generate audio sample
	int sample[4];
	int leftSample = 0;
	int rightSample = 0;

	for (int i = 0; i < 4; i++)
	{
		sample[i] = 0;

	}

	if ((div % 16) == 0)
	{
		
		for (int i = 0; i < 4; i++)
		{
			if (channels[i] != nullptr)
			{
				sample[i] = channels[i]->generateSample();

			}
		}

		//panning
		for (int i = 0; i < 4; i++)
		{
			if (BIT(regs.NR51, (4 + i)))
			{
				leftSample += sample[i];
			}

			if (BIT(regs.NR51, i))
			{
				rightSample += sample[i];
			}

		}
		leftSample = leftSample / 4;
		rightSample = rightSample / 4;

		uint8_t outSamples[2];
		outSamples[0] = (uint8_t)leftSample;
		outSamples[1] = (uint8_t)rightSample;

		if ((leftSample != 0x80) && (rightSample != 0x80))
		{
			std::cout << "Non-zero sample" << std::endl;
		}



		aui->putAudio(outSamples, 2);
		//uint8_t buff[2] = { 0xFF,0x80 };
		//aui->putAudio(buff, 2);
	}
	

	// 
	//for (int i = 0; i < 4; i++)
	//{
	//	if (channels[i] != nullptr)
	//	{
	//		int sample = channels[i]->generateSample();
	//		aui->putAudio(&sample, 1);
	//	}
	//}

	prev_div = div;

}

void apu::div_apu_tick()
{
	div_apu++;

	if (!apuOn)
	{
		return;
	}

	if ((div_apu % 2) == 0)  // Sound Length Sweep
	{
		for (int i = 0; i < 4; i++)
		{
			if (channels[i] != nullptr)
			{
				if (channels[i]->lengthEnabled)
				{
					channels[i]->lengthTick();
				}
			}
		}
	}
	if ((div_apu % 8) == 0)  // Envelope Sweep
	{
		for (int i = 0; i < 4; i++)
		{
			if (channels[i] != nullptr)
			{
				channels[i]->envelopeSweepTick();
			}
		}
	}

}

void apu::apu_on()
{
	apuOn = true;
}
void apu::apu_off()
{
	apuOn = false;
	memset(&regs, 0, sizeof(regs));
	channels[1]->channelAPUOff();
}
