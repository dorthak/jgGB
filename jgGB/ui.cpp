#include "ui.h"
#include "emu.h"

ui::ui()
{

}
ui::ui(emu* e)
{
	this->em = e;
}
ui::~ui()
{

}
void ui::set_emu(emu* e)
{
	this->em = e;
}


void ui::ui_init()
{
	SDL_Init(SDL_INIT_VIDEO);
	std::cout << "SDL INIT" << std::endl;
	TTF_Init();
	std::cout << "TTF INIT" << std::endl;

	SDL_CreateWindowAndRenderer("jgGB", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlWindow, &sdlRenderer);
}
void ui::ui_handle_events()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		//TODO SDL_UpdateWindowSurface(sdlWindow);
		//TODO SDL_UpdateWindowSurface(sdlTraceWindow);
		//TODO SDL_UpdateWindowSurface(sdlDebugWindow);

		if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED || e.type == SDL_EVENT_WINDOW_DESTROYED) 
		{
			em->kill();
		}
	}
}

void ui::delay(uint32_t ms)
{
	SDL_Delay(ms);
}
