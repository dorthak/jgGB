#include "common.h"
#include "emu.h"
#include "cpu.h"
#include <thread>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>




emu::emu()
{
	paused = false;
	running = false;
	die = false;
	ticks = 0;

	b = new bus();
	s = new stack(b);
	crt = new cart();
	c = new cpu(b, this, s);
	r = new ram();
	u = new ui(this);
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

void emu::kill()
{
	die = true;
}

bool emu::is_paused()
{
	return paused;
}
bool emu::is_running()
{
	return running;
}

int emu::emu_cart_load(int argc, char** argv)
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
	
	u->ui_init();

	std::thread cpu_thread(&emu::run_cpu, this);
	cpu_thread.detach();

	while (!die)
	{
		u->delay(1);
		u->ui_handle_events();
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

void emu::run_cpu()
{
	running = true;
	paused = false;
	ticks = 0;

	while (running)
	{
		if (die)
		{
			return;
		}

		if (paused)
		{
			u->delay(10);
			continue;
		}

		if (!c->cpu_step())
		{
			std::cout << "CPU Stopped" << std::endl;
			return;
		}

		ticks++;

		if (die)
		{
			return;
		}
	}
}