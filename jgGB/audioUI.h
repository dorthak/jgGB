#pragma once
#include <SDL3/SDL.h>
#include "common.h"


class ui;

class audioUI
{
public:
	audioUI(ui* u);
	~audioUI();

	void putAudio(const void *buf, int len);
	void audioUIinit();
private:
	ui* u;
	SDL_AudioStream* mixstream;
	SDL_AudioStream* outstream;

	static void SDLCALL OutStreamCallback(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount);


};