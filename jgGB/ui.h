#pragma once
#include "common.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


class emu;

class ui
{
public:
	ui();
	ui(emu* e);
	~ui();
	void ui_init();
	void ui_handle_events();
	
	void set_emu(emu* e);

	void delay(uint32_t ms);

private:
	emu* em;

	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;

	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Surface* screen;
};

