#include "audioUI.h"
#include "ui.h"

audioUI::audioUI(ui* u)
{
	this->u = u;
}

void audioUI::audioUIinit()
{
	SDL_AudioSpec inspec;
	inspec.format = SDL_AUDIO_U8;
	inspec.channels = 1;
	inspec.freq = AUDIOFREQ;

	SDL_AudioSpec outspec;
	outspec.format = SDL_AUDIO_U8;
	outspec.channels = 1;
	outspec.freq = 48000;

	mixstream = SDL_CreateAudioStream(&inspec, &outspec);
	if (mixstream == NULL)
	{
		std::cerr << "Error creating mixing audio stream - " << SDL_GetError() << std::endl;
	}
	outstream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &outspec, audioUI::OutStreamCallback, this);
	if (outstream == NULL)
	{
		std::cerr << "Error creating output audio stream - " << SDL_GetError() << std::endl;
		return;
	}
	std::cout << "Output audio stream initalized" << std::endl;

	bool result = false;
	result = SDL_ResumeAudioStreamDevice(outstream);
	if (!result)
	{
		std::cerr << "Unable to resume output audio stream - " << SDL_GetError() << std::endl;
	}

}
audioUI::~audioUI()
{
	SDL_DestroyAudioStream(mixstream);
	SDL_DestroyAudioStream(outstream);
}

void audioUI::putAudio(const void* buf, int len)
{
	bool result = false;
	result = SDL_PutAudioStreamData(mixstream, buf, len);
	if (!result)
	{
		std::cerr << "Unable to push audio data" << std::endl;
	}

}

void SDLCALL audioUI::OutStreamCallback(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount)
{
	audioUI* aui = (audioUI*)userdata;

	int addSamples = additional_amount / sizeof(uint8_t);
	while (addSamples > 0)
	{
		uint8_t samples[64];
		const int total = SDL_min(addSamples, SDL_arraysize(samples));

		int receivedSamples = SDL_GetAudioStreamData(aui->mixstream, samples, total * sizeof(uint8_t));
		
		if (receivedSamples < 0)
		{
			std::cerr << "Failed to get audio samples - " << SDL_GetError() << std::endl;
			return;
		}

		SDL_PutAudioStreamData(aui->outstream, samples, receivedSamples * sizeof(uint8_t));

		addSamples -= receivedSamples * sizeof(uint8_t);

	}
	
}