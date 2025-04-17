#pragma once
#include "common.h"
#include "emu.h"
#include "cart.h"
#include "bus.h"

class cpu;

class emu
{
public:
	emu();
	~emu();

	int emu_start(int argc, char** argv);
	int emu_run();

	bool is_paused();
	bool is_running();
	
	void emu_cycles(int cpu_cycles);
	uint64_t get_ticks();

private:
	bus *b;
	cpu *c;
	cart *crt;
	
	bool paused;
	bool running;
	uint64_t ticks;
};

