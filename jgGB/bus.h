#pragma once
#include "common.h"
#include "cart.h"
#include "instdata.h"

class cpu;
class ram;
class io;
class timer;
class dbg;
class ppu;
class lcd;
class emu;


class bus
{
public:

	bus();
	uint8_t bus_read(uint16_t address);
	void bus_write(uint16_t address, uint8_t value);
	void bus_write16(uint16_t address, uint16_t value);
	uint16_t bus_read16(uint16_t address);
	void set_cart(cart* crt);
	void set_ram(ram* r);
	void set_cpu(cpu* c);
	void set_io(io* i);
	void set_timer(timer* t);
	void set_debug(dbg* d);
	void set_ppu(ppu* p);
	void set_lcd(lcd* l);
	void set_emu(emu* e);

	//bus passing messages between its members
	uint8_t bus_get_cpu_int_flags();
	void bus_set_cpu_int_flags(uint8_t value);
	void bus_request_cpu_interrupt(instdata::interrupt_type t);

	uint8_t bus_timer_read(uint16_t  address);
	void bus_timer_write(uint16_t address, uint8_t value);
	void bus_timer_tick();

	void bus_dbg_update();
	void bus_dbg_print();

	//DMA commands
	void dma_start(uint8_t start);
	void dma_tick();
	bool dma_transferring();

	//cart save commands
	bool bus_cart_need_save();
	bool bus_cart_battery();
	void bus_cart_battery_load();
	void bus_cart_battery_save();

	//RTC Commands
	void bus_rtc_tick();
	uint64_t bus_get_ticks();
	uint64_t bus_get_ticks_ns();


private:
	cart* crt;
	ram* r;
	cpu* c;
	io* i;
	timer* t;
	dbg* d;
	ppu* p;
	lcd* l;
	emu* e;


	//DMA status
	bool dma_active;
	uint8_t dma_cur_byte;
	uint8_t dma_cur_value;
	uint8_t dma_start_delay;
};

