#pragma once
#include "common.h"
#include "emu.h"
#include "cart.h"
#include "bus.h"
#include "ram.h"
#include "stack.h"
#include "ui.h"

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

	void kill();

private:
	bus *b;
	cpu *c;
	cart *crt;
	ram* r;
	stack* s;
	ui* u;
	
	bool die;
	bool paused;
	bool running;
	uint64_t ticks;

	void run_cpu();
};

