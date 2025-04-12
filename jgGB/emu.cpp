#include "common.h"
#include "emu.h"
#include "cart.h"
#include "cpu.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void delay(uint32_t ms)
{
	SDL_Delay(ms);
}



emu::emu()
{
	this->paused = false;
	this->running = false;
	this->ticks = 0;
}

bool emu::is_paused()
{
	return this->paused;
}
bool emu::is_running()
{
	return this->running;
}


int emu::emu_run(int argc, char** argv)
{
	cart c = cart();
	cpu cp = cpu();
	if (argc < 2)
	{
		std::cout << "Usage: emu <rom_file>" << std::endl;
		return -1;
	}

	if (!(c.cart_load(argv[1])))
	{
		printf("Failed to load ROM file: %s\n", argv[1]);
		return -2;
	}

	printf("Cart loaded..\n");

	SDL_Init(SDL_INIT_VIDEO);
	printf("SDL INIT\n");
	TTF_Init();
	printf("TTF INIT\n");

	cp.cpu_init();

	this->running = true;
	this->paused = false;

	while (this->running)
	{
		if (this->paused)
		{
			delay(10);
			continue;
		}

		if (!cp.cpu_step())
		{
			std::cout << "CPU Stopped" << std::endl;
			return -3;
		}
		
		this->ticks++;
	}
	
	return 0;
}
