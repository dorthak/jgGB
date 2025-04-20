#include "bus.h"
#include "ram.h"
#include "cpu.h"
#include "io.h"
#include "timer.h"
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
	if (crt == 0)
	{
		std::cout << "ERROR: Cart not connected to bus." << std::endl;
		return 0;
	}

	if (address < 0x8000)
	{
		//ROM DATA
		return crt->cart_read(address);
	}
	else if (address < 0xA000) {
		//Char/Map Data
		//TODO
		printf("UNSUPPORTED bus_read(%04X)\n", address);
		//NO_IMPL
		return 0;
	} else if (address < 0xC000) {
		//Cartridge RAM
		return crt->cart_read(address);
	} else if (address < 0xE000) {
		//WRAM
		return r->wram_read(address);
	} else if (address < 0xFE00) {
		//reserved echo ram
		return 0;
	} else if (address < 0xFEA0) {
		//OAM
		//TODO
		printf("UNSUPPORTED bus_read(%04X)\n", address);
		//NO_IMPL
		return 0;
	} else if (address < 0xFF00) {
		//reserved unsuable
		return 0;
	} else if (address < 0xFF80) {
		//IO Registers...
		//TODO
		//printf("UNSUPPORTED bus_read(%04X)\n", address);
		//NO_IMPL
		return i->io_read(address);
		
	} else if (address == 0xFFFF) {
		//CPU ENABLE REGISTER
		//TODO
		return c->cpu_get_ie_register();
	} else {
		return r->hram_read(address);
	}
}

uint16_t bus::bus_read16(uint16_t address)
{
	uint16_t lo = bus_read(address);
	uint16_t hi = bus_read(address);

	return lo | (hi << 8);
}

void bus::bus_write(uint16_t address, uint8_t value)
{
	if (crt == 0)
	{
		std::cout << "ERROR: Cart not connected to bus." << std::endl;
	}

	if (address < 0x8000)
	{
		//ROM DATA
		crt->cart_write(address, value);
	}
	else if (address < 0xA000) {
		//Char/Map Data
		//TODO
		printf("UNSUPPORTED bus_write(%04X)\n", address);
		//NO_IMPL
	} else if (address < 0xC000) {
		//Cartridge RAM
		crt->cart_write(address, value);
	} else if (address < 0xE000) {
		//WRAM
		r->wram_write(address, value);
	} else if (address < 0xFE00) {
		//reserved echo ram
		
	} else if (address < 0xFEA0) {
		//OAM
		//TODO
		printf("UNSUPPORTED bus_write(%04X)\n", address);
		//NO_IMPL
	} else if (address < 0xFF00) {
		//reserved unsuable
	} else if (address < 0xFF80) {
		//IO Registers...
		//TODO
		//printf("UNSUPPORTED bus_write(%04X)\n", address);
		//NO_IMPL
		i->io_write(address, value);
	} else if (address == 0xFFFF) {
		//CPU ENABLE REGISTER
		//TODO
		c->cpu_set_ie_register(value);
	} else {
		r->hram_write(address, value);
	}

	
}

void bus::bus_write16(uint16_t address, uint16_t value)
{
	bus_write(address + 1, (value >> 8) & 0xFF);
	bus_write(address, value & 0xFF);

}

void bus::set_cart(cart* crt)
{
	this->crt = crt;
}

void bus::set_ram(ram* r)
{
	this->r = r;
}

void bus::set_cpu(cpu* c)
{
	this->c = c;
}

void bus::set_io(io* i)
{
	this->i = i;
}

void bus::set_timer(timer* t)
{
	this->t = t;
}

uint8_t bus::bus_get_cpu_int_flags()
{
	return c->cpu_get_int_flags();
}

void bus::bus_set_cpu_int_flags(uint8_t value)
{
	c->cpu_set_int_flags(value);
}

uint8_t bus::bus_timer_read(uint16_t address)
{
	return t->timer_read(address);
}

void bus::bus_timer_write(uint16_t address, uint8_t value)
{
	t->timer_write(address, value);
}

void bus::bus_timer_tick()
{
	t->timer_tick();
}