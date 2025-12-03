#include "audioUI.h"
#include "ui.h"

audioUI::audioUI(ui* u)
{
	this->u = u;
}

void audioUI::audioUIinit()
{
	SDL_AudioSpec inspec;
	inspec.format = SDL_AUDIO_S16;
	inspec.channels = 2;
	inspec.freq = AUDIOFREQ;

	SDL_AudioSpec outspec;
	outspec.format = SDL_AUDIO_S16;
	outspec.channels = 2;
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

	int addSamples = additional_amount / sizeof(int16_t);

	int16_t samples[480];
	const int total = SDL_min(addSamples, SDL_arraysize(samples));

	int receivedSamples = SDL_GetAudioStreamData(aui->mixstream, samples, total * sizeof(int16_t));
		
	if (receivedSamples < 0)
	{
		std::cerr << "Failed to get audio samples - " << SDL_GetError() << std::endl;
		return;
	}

	//if (receivedSamples == 0)  //get some zeroes into the queue when first starting up
	//{
	//	//memset(samples, 0x80, sizeof(samples));
	//	std::cout << "No samples in queue" << std::endl;

	//}
		
	// std::cout << "Samples requested: "<< addSamples << " Samples recieved: " << receivedSamples << std::endl;
	
	SDL_PutAudioStreamData(aui->outstream, samples, receivedSamples * sizeof(uint8_t));

	
}