#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class PointLightObject
	: public SceneObject
{
public:
	PointLightObject(std::string name, vec3 color, vec3 position, float radius);

	~PointLightObject();

public:
	inline vec3 getPosition() { return m_position; }
	inline void setPosition(vec3 position) { m_position = position; }
	inline vec3 getColor() { return m_color; }
	inline void setColor(float radius) { m_radius = radius; }
	inline float getRadius() { return m_radius; }
	inline void setRadius(float radius) { m_radius = radius; }

private:
	vec3 m_position;
	vec3 m_color;
	float m_radius;
};