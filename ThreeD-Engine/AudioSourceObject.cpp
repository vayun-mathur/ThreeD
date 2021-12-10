#include "AudioSourceObject.h"
#include <iostream>
#include "AudioSound.h"
#include "alhelper.h"
#include "SceneSystem.h"
#include "CameraObject.h"

AudioSourceObject::AudioSourceObject(std::string name, SceneSystem* system, vec3 position) 
	: SceneObject(name, system), m_position(position)
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
	vec4 pos = m_system->getCamera()->getCameraPosition();
	alCall(alSource3f, source, AL_POSITION, m_position.x - pos.x, m_position.y - pos.y, m_position.z - pos.z);
	alCall(alSourcef, source, AL_PITCH, m_pitch);
	alCall(alSourcef, source, AL_GAIN, m_gain);
}