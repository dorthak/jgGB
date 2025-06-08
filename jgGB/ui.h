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
	void ui_update();
	
	void set_emu(emu* e);

	void delay(uint32_t ms);
	uint32_t get_ticks();
	

private:
	emu* em;

	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;

	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Surface* screen;

	const int debugScale = 4;

	const unsigned long tile_colors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

	SDL_Window* sdlDebugWindow;
	SDL_Renderer* sdlDebugRenderer;
	SDL_Texture* sdlDebugTexture;
	SDL_Surface* debugScreen;

	void display_tile(SDL_Surface* surface, uint16_t startLocation, uint16_t tileNum, int x, int y);

	void update_dbg_window();

};

