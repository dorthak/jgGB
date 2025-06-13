#include "bus.h"
#include "ram.h"
#include "cpu.h"
#include "io.h"
#include "timer.h"
#include "dbg.h"
#include "ppu.h"
#include "lcd.h"

#include <windows.h>
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
	dma_active = false;
	dma_cur_byte = 0;
	dma_cur_value = 0;
	dma_start_delay = 0;

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
		return p->ppu_vram_read(address);
	} else if (address < 0xC000) {
		//Cartridge RAM
		return crt->cart_read(address);
	} else if (address < 0xE000) {
		//WRAM
		return r->wram_read(address);
	} else if (address < 0xFE00) {
		//reserved echo ram
		return r->wram_read(address - 0x2000);
	} else if (address < 0xFEA0) {
		//OAM
		return p->ppu_oam_read(address);
	} else if (address < 0xFF00) {
		//reserved unsuable
		return 0;
	} else if (address < 0xFF80) {
		//IO Registers...
		if ((address >= 0xFF40) && (address <= 0xFF4B))
		{
			return l->lcd_read(address);
		}
		return i->io_read(address);
		
	} else if (address == 0xFFFF) {
		//CPU ENABLE REGISTER
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
		p->ppu_vram_write(address, value);
	} else if (address < 0xC000) {
		//Cartridge RAM
		crt->cart_write(address, value);
	} else if (address < 0xE000) {
		//WRAM
		r->wram_write(address, value);
	} else if (address < 0xFE00) {
		//reserved echo ram
		r->wram_write(address - 0x2000, value);
		
	} else if (address < 0xFEA0) {
		//OAM
		//TODO
		//printf("UNSUPPORTED bus_write(%04X)\n", address);
		//NO_IMPL
		p->ppu_oam_write(address, value);
	} else if (address < 0xFF00) {
		//reserved unsuable

	} else if (address < 0xFF40) {
		//IO Registers...
		i->io_write(address, value);
	} else if (address < 0xFF4C) {
		//LCD Registers
		l->lcd_write(address, value);
	} else if (address < 0xFF80) {
		//IO Registers
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
void bus::set_debug(dbg* d)
{
	this->d = d;
}
void bus::set_ppu(ppu* p)
{
	this->p = p;
}
void bus::set_lcd(lcd* l)
{
	this->l = l;
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

void bus::bus_request_cpu_interrupt(instdata::interrupt_type t)
{
	c->cpu_request_interrupt(t);
}

void bus::bus_dbg_update()
{
	d->dbg_update();
}
void bus::bus_dbg_print()
{
	d->dbg_print();
}


void bus::dma_start(uint8_t start)
{
	dma_active = true;
	dma_cur_byte = 0;
	dma_start_delay = 2;
	dma_cur_value = start;
}
void bus::dma_tick()
{
	if (!dma_active)
	{
		return;
	}
	if (dma_start_delay)
	{
		dma_start_delay--;
		return;
	}

	p->ppu_oam_write(dma_cur_byte, bus_read((dma_cur_value * 0x100) + dma_cur_byte));
	dma_cur_byte++;
	dma_active = dma_cur_byte < 0xA0;

	//if (!dma_active)
	//{
	//	std::cout << "DMA DONE!" << std::endl;
	//	Sleep(2);
	//}
	
}
bool bus::dma_transferring()
{
	return dma_active;
}