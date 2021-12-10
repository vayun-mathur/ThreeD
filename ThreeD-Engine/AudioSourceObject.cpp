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

void AudioSourceObject::loop(bool looping)
{
	alCall(alSourcei, source, AL_LOOPING, looping?AL_TRUE:AL_FALSE);
}

void AudioSourceObject::update(double delta_time)
{
	alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);
	vec3 campos = m_system->getCamera()->getCameraPosition();
	vec3 curr_relpos = m_position - campos;
	vec3 prev_relpos;
	alCall(alGetSource3f, source, AL_POSITION, &prev_relpos.x, &prev_relpos.y, &prev_relpos.z);
	alCall(alSource3f, source, AL_POSITION, curr_relpos.x, curr_relpos.y, curr_relpos.z);
	vec3 velocity = (curr_relpos - prev_relpos) * (1/delta_time);
	alCall(alSource3f, source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	alCall(alSourcef, source, AL_PITCH, m_pitch);
	alCall(alSourcef, source, AL_GAIN, m_gain);
}