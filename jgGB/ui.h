#pragma once
#include "common.h"

#include <SDL3/SDL.h>
//#include <SDL3_ttf/SDL_ttf.h>


class emu;
class ppu;
class io;

class ui
{
public:
	ui();
	ui(emu* e, io* i);
	~ui();
	void ui_init();
	void ui_handle_events();
	void ui_update();
	
	void set_emu(emu* e);
	void set_ppu(ppu* p);

	void delay(uint32_t ms);
	void delay_ns(uint64_t ns);
	uint64_t get_ticks();
	uint64_t get_ticks_ns();


private:
	emu* em;
	ppu* p;
	io* i;

	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;

	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Surface* screen;

	const int scale = 4;
	const int debugScale = 4;

	const unsigned long tile_colors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

	SDL_Window* sdlDebugWindow;
	SDL_Renderer* sdlDebugRenderer;
	SDL_Texture* sdlDebugTexture;
	SDL_Surface* debugScreen;

	void display_tile(SDL_Surface* surface, uint16_t startLocation, uint16_t tileNum, int x, int y);

	void update_dbg_window();

	void ui_on_key(bool down, SDL_Keycode key_code);

};

