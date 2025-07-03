#include "common.h"
#include "emu.h"
#include "cpu.h"
#include <thread>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


//forward declare class
class SST;


emu::emu()
{
	paused = false;
	running = false;
	die = false;
	ticks = 0;

	b = new bus();
	s = new stack(b);
	crt = new cart(b);
	c = new cpu(b, this, s);
	r = new ram();
	i = new io(b);
	u = new ui(this, i);
	t = new timer(b);
	d = new dbg(b);
	l = new lcd(b);
	p = new ppu(b, l, u);
	a = new apu(b, u);


	s->set_cpu(c);
	b->set_cpu(c);
	b->set_ram(r);
	b->set_timer(t);
	b->set_io(i);
	b->set_debug(d);
	b->set_ppu(p);
	b->set_lcd(l);
	b->set_emu(this);
	b->set_apu(a);
	u->set_ppu(p);
}

emu::~emu()
{
	delete c;
	delete s;
	delete crt;
	delete b;
	delete p;
	delete d;
	delete t;
	delete i;
	delete u;
	delete r;
	delete a;


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

	uint32_t prev_frame = 0;

	while (!die)
	{
		u->delay(1);
		u->ui_handle_events();
		if (prev_frame != p->ppu_get_current_frame())
		{
			u->ui_update();
		}
		prev_frame = p->ppu_get_current_frame();
	}
	
	return 0;
}

void emu::emu_cycles(int cpu_cycles)
{
	
	for (int i = 0; i < cpu_cycles; i++)
	{
		for (int n = 0; n < 4; n++)
		{
			ticks++;
			b->bus_timer_tick();
			p->ppu_tick();
			b->bus_rtc_tick();  //TODO: is this the right place for it?

#ifdef LIMIT_CYCLE_T
			uint64_t end = u->get_ticks_ns();
			uint64_t cycle_time = end - prev_cycle_time;

			if (cycle_time < target_cycle_time)
			{
				u->delay_ns((target_cycle_time - cycle_time));
			}
#endif
		}
		b->dma_tick();

		//M-cycle speed limiter
#ifdef LIMIT_CYCLE_M
		uint64_t end = u->get_ticks_ns();
		uint64_t cycle_time = end - prev_cycle_time;

		if (cycle_time < target_cycle_time)
		{
			u->delay_ns((target_cycle_time - cycle_time));
		}
#endif
	}

}

uint64_t emu::get_ticks()
{
	return ticks;
}

uint64_t emu::get_sys_ticks()
{
	return u->get_ticks();
}
uint64_t emu::get_sys_ticks_ns()
{
	return u->get_ticks_ns();
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

	}
}