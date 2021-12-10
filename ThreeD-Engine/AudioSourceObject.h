#pragma once
#include <al.h>
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"

class AudioSourceObject
	: public SceneObject
{
public:
	AudioSourceObject(std::string name, vec3 position);
	~AudioSourceObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::AudioSourceObject;
	}
public:
	void play(AudioSoundPtr sound);
private:
	virtual void update(double delta_time);
private:
	vec3 m_position;
private:
	ALuint source;
	ALint state;
};

