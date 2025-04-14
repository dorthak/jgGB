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
	if (argc < 2)
	{
		std::cout << "Usage: emu <rom_file>" << std::endl;
		return -1;
	}
	cart c = cart(argv[1]);

	if (!(c.cart_loaded()))
	{
		printf("Failed to load ROM file: %s\n", argv[1]);
		return -2;
	}
	bus b = bus();
	b.set_cart(&c);

	cpu cp = cpu(&b, this);

	printf("Cart loaded..\n");

	SDL_Init(SDL_INIT_VIDEO);
	printf("SDL INIT\n");
	TTF_Init();
	printf("TTF INIT\n");

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

void emu::emu_cycles(int cpu_cycles)
{
	//TODO
}