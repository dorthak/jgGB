#pragma once
#include "common.h"\


class emu
{
public:
	emu();

	int emu_run(int argc, char** argv);

	bool is_paused();
	bool is_running();
	

private:

	bool paused;
	bool running;
	uint64_t ticks;
};

