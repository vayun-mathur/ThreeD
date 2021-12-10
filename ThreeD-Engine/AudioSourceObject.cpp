#include "AudioSourceObject.h"
#include <iostream>
#include "AudioSound.h"
#include "alhelper.h"

AudioSourceObject::AudioSourceObject(std::string name, vec3 position) : SceneObject(name), m_position(position)
{
	alCall(alGenSources, 1, &source);
	alCall(alSourcef, source, AL_PITCH, 1);
	alCall(alSourcef, source, AL_GAIN, 1.0f);
	alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
	alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
	alCall(alSourcei, source, AL_LOOPING, AL_FALSE);
	alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);
}

AudioSourceObject::~AudioSourceObject()
{
	alCall(alDeleteSources, 1, &source);
}

void AudioSourceObject::play(AudioSoundPtr sound)
{
	alCall(alSourcei, source, AL_BUFFER, sound->buffer);
	alCall(alSourcePlay, source);
}

void AudioSourceObject::update(double delta_time)
{
	alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);
	alCall(alSource3f, source, AL_POSITION, m_position.x, m_position.y, m_position.z);
}