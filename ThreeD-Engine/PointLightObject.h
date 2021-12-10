#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class PointLightObject
	: public SceneObject
{
public:
	PointLightObject(std::string name, SceneSystem* system, vec3 color, vec3 position, float radius, vec3 attenuation);

	~PointLightObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::PointLightObject;
	}

	virtual ScriptValue* dot(std::string);
public:
	inline vec3 getPosition() { return m_position; }
	inline void setPosition(vec3 position) { m_position = position; }
	inline vec3 getColor() { return m_color; }
	inline void setColor(vec3 color) { m_color = color; }
	inline vec3 getAttenuation() { return m_attenuation; }
	inline void setAttenuation(vec3 attenuation) { m_attenuation = attenuation; }
	inline float getRadius() { return m_radius; }
	inline void setRadius(float radius) { m_radius = radius; }

private:
	vec3 m_position;
	vec3 m_color;
	float m_radius;
	vec3 m_attenuation;
};