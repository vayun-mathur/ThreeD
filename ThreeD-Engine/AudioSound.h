#pragma once
#include "Resource.h"
#include <al.h>

class AudioSound : public Resource
{
public:
	AudioSound(const wchar_t* full_path);
	~AudioSound();

	void play();
	void update();
private:
	ALuint source;
	ALuint buffer;

	ALint state;
};

