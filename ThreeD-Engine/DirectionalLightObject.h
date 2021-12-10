#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class DirectionalLightObject
	: public SceneObject
{
public:
	DirectionalLightObject(std::string name, SceneSystem* system, vec3 color, vec3 direction);

	~DirectionalLightObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::DirectionalLightObject;
	}

public:
	inline vec3 getDirection() { return m_direction; }
	inline void setDirection(vec3 direction) { m_direction = direction; }
	inline vec3 getColor() { return m_color; }
	inline void setColor(vec3 color) { m_color = color; }
	
private:
	vec3 m_direction;
	vec3 m_color;
};