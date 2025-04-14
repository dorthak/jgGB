#pragma once
#include "common.h"


class emu
{
public:
	emu();

	int emu_run(int argc, char** argv);

	bool is_paused();
	bool is_running();
	
	void emu_cycles(int cpu_cycles);

private:

	bool paused;
	bool running;
	uint64_t ticks;
};

