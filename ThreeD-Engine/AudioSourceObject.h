#pragma once
#include <al.h>
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"

class AudioSourceObject
	: public SceneObject
{
public:
	AudioSourceObject(std::string name, SceneSystem* system, vec3 position);
	~AudioSourceObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::AudioSourceObject;
	}
public:
	void play(AudioSoundPtr sound);
public:
	inline vec3 getPosition() { return m_position; }
	inline void setPosition(vec3 position) { m_position = position; }
	inline float getPitch() { return m_pitch; }
	inline void setPitch(float pitch) { m_pitch = pitch; }
	inline float getGain() { return m_gain; }
	inline void setGain(float gain) { m_gain = gain; }
private:
	virtual void update(double delta_time);
private:
	vec3 m_position;
	float m_pitch;
	float m_gain;
private:
	ALuint source;
	ALint state;
};

