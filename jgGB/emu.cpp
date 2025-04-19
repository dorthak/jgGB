#include "common.h"
#include "emu.h"
#include "cpu.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void delay(uint32_t ms)
{
	SDL_Delay(ms);
}



emu::emu()
{
	paused = false;
	running = false;
	ticks = 0;

	b = new bus();
	s = new stack(b);
	crt = new cart();
	c = new cpu(b, this, s);
	r = new ram();
	b->set_cpu(c);
	b->set_ram(r);
	s->set_cpu(c);
}

emu::~emu()
{
	delete(c);
	delete(crt);
	delete(b);

}

bool emu::is_paused()
{
	return paused;
}
bool emu::is_running()
{
	return running;
}

int emu::emu_start(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: emu <rom_file>" << std::endl;
		return -1;
	}
	crt->cart_load(argv[1]);

	if (!(crt->cart_loaded()))
	{
		printf("Failed to load ROM file: %s\n", argv[1]);
		return -2;
	}

	b->set_cart(crt);


	printf("Cart loaded..\n");
	return 0;
}

int emu::emu_run()
{


	SDL_Init(SDL_INIT_VIDEO);
	printf("SDL INIT\n");
	TTF_Init();
	printf("TTF INIT\n");

	running = true;
	paused = false;

	while (running)
	{
		if (paused)
		{
			delay(10);
			continue;
		}

		if (!c->cpu_step())
		{
			std::cout << "CPU Stopped" << std::endl;
			return -3;
		}
		
		ticks++;
	}
	
	return 0;
}

void emu::emu_cycles(int cpu_cycles)
{
	//TODO
}

uint64_t emu::get_ticks()
{
	return ticks;
}