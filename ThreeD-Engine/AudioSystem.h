#pragma once
#include "prerequisites.h"
#include <alc.h>

class AudioSystem
{
public:
	AudioSystem();
	~AudioSystem();
public:
	AudioSoundManager* getAudioSoundManager() { return m_audiosound_manager; }
public:
	static AudioSystem* get();
	static void create();
	static void release();
private:
	ALCdevice* openALDevice;
	ALCcontext* openALContext;
	ALCboolean contextMadeCurrent;
	AudioSoundManager* m_audiosound_manager = nullptr;
private:
	static AudioSystem* m_system;
};