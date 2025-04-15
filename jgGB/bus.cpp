#include "bus.h"
// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

bus::bus()
{

}

uint8_t bus::bus_read(uint16_t address)
{
	if (this->crt == 0)
	{
		std::cout << "ERROR: Cart not connected to bus." << std::endl;
		return 0;
	}

	if (address < 0x8000)
	{
		return this->crt->cart_read(address);
	}

	NO_IMPL
		return 0;

}
void bus::bus_write(uint16_t address, uint8_t value)
{
	if (this->crt == 0)
	{
		std::cout << "ERROR: Cart not connected to bus." << std::endl;

	}
	if (address < 0x8000)
	{
		this->crt->cart_write(address, value);
	}
}

void bus::set_cart(cart* crt)
{
	this->crt = crt;
}