#pragma once
#include "common.h"
#include "emu.h"
#include "cart.h"
#include "bus.h"
#include "ram.h"
#include "stack.h"
#include "ui.h"
#include "io.h"
#include "timer.h"
#include "dbg.h"
#include "ppu.h"
#include "lcd.h"
#include "apu.h"

class cpu;

class emu
{
public:
	emu();
	~emu();

	int emu_cart_load(int argc, char** argv);
	int emu_run();

	bool is_paused();
	bool is_running();
	
	void emu_cycles(int cpu_cycles);
	uint64_t get_ticks();

	uint64_t get_sys_ticks();
	uint64_t get_sys_ticks_ns();

	void kill();
	bus* b;

private:

	cpu *c;
	cart *crt;
	ram* r;
	stack* s;
	ui* u;
	io* i;
	timer* t;
	dbg* d;
	ppu* p;
	lcd* l;
	apu* a;
	
	bool die;
	bool paused;
	bool running;
	uint64_t ticks;

	void run_cpu();
	friend class SST;
#ifdef LIMIT_CYCLE_M
	const uint64_t target_cycle_time = 238 * 4; //ns, m-cycle
#endif
#ifdef LIMIT_CYCLE_T
	const uint64_t target_cycle_time = 238; //ns, T-cycle
#endif

	uint64_t prev_cycle_time = 0;
};

