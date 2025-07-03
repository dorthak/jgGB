#include "apu.h"
#include "bus.h"
#include "ui.h"
#include "aChannelBase.h"
#include "aChannel2.h"

apu::apu(bus* b, ui* u)
{
	this->b = b;
	this->u = u;
	
	//init channels
	for (int i = 0;i < 4;i++)
	{
		channels[i] = nullptr;
	}
	aChannel2 *ch2 = new aChannel2(this);
	channels[2] = ch2;
	
	

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
		uint8_t offset = (address - 0xFF10);
		uint8_t* p = (uint8_t*)&regs;
		p[offset] = value;
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