#pragma once
#include "ResourceManager.h"

class AudioSoundManager : public ResourceManager
{
public:
	AudioSoundManager();
	~AudioSoundManager();
	AudioSoundPtr createAudioSoundFromFile(const wchar_t* file_path);

protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path) override;
};

#pragma once
