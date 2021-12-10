#pragma once
#include "Resource.h"
#include <al.h>
#include "AudioSourceObject.h"

class AudioSound : public Resource
{
public:
	AudioSound(const wchar_t* full_path);
	~AudioSound();
private:
	ALuint buffer;
	friend class AudioSourceObject;
};

