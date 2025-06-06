#include "ui.h"
#include "emu.h"
#include "bus.h"

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

	SDL_CreateWindowAndRenderer("jgGB Debug", (16 * 8 * debugScale), (32 * 8 * debugScale), 0, &sdlDebugWindow, &sdlDebugRenderer);

	SDL_PixelFormat pixelDebugFormat = SDL_GetPixelFormatForMasks(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	int debugWidth = (16 * 8 * debugScale) + (16 * debugScale);
	int debugHeight = (32 * 8 * debugScale) + (64 * debugScale);

	debugScreen = SDL_CreateSurface(debugWidth, debugHeight, pixelDebugFormat);

	sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, debugWidth, debugHeight);

	int x, y;
	SDL_GetWindowPosition(sdlWindow, &x, &y);
	SDL_SetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);


}
void ui::ui_update()
{
	update_dbg_window();
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


void ui::display_tile(SDL_Surface *surface, uint16_t startLocation, uint16_t tileNum, int x, int y)
{
	SDL_Rect rc;

	for (int tileY = 0; tileY < 16; tileY += 2)
	{
		uint8_t b1 = em->b->bus_read(startLocation + (tileNum * 16) + tileY);
		uint8_t b2 = em->b->bus_read(startLocation + (tileNum * 16) + tileY + 1);

		for (int bit = 7; bit >= 0; bit--)
		{
			uint8_t hi = !!(b1 & (1 << bit)) << 1;
			uint8_t lo = !!(b2 & (1 << bit));

			uint8_t color = hi | lo;

			rc.x = x + ((7 - bit) * debugScale);
			rc.y = y + (tileY / 2 * debugScale);
			rc.w = debugScale;
			rc.h = debugScale;

			SDL_FillSurfaceRect(surface, &rc, tile_colors[color]);

		}
	}
}
void ui::update_dbg_window()
{
	int xDraw = 0;
	int yDraw = 0;
	int tileNum= 0;

	SDL_Rect rc;
	rc.x = 0;
	rc.y = 0;
	rc.w = debugScreen->w;
	rc.h = debugScreen->h;
	SDL_FillSurfaceRect(debugScreen, &rc, 0xFF111111);

	uint16_t addr = 0x8000;

	//384 tiles, 24 x 16

	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			display_tile(debugScreen, addr, tileNum, xDraw + (x * debugScale), yDraw + (y * debugScale));
			xDraw += (8 * debugScale);
			tileNum++;
		}

		yDraw += (8 * debugScale);
		xDraw = 0;
	}

	SDL_UpdateTexture(sdlDebugTexture, NULL, debugScreen->pixels, debugScreen->pitch);
	SDL_RenderClear(sdlDebugRenderer);
	SDL_RenderTexture(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
	SDL_RenderPresent(sdlDebugRenderer);
	
}