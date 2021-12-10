#include "DirectionalLightObject.h"

DirectionalLightObject::DirectionalLightObject(std::string name, SceneSystem* system, vec3 color, vec3 direction)
	: SceneObject(name, system), m_color(color), m_direction(direction)
{
	m_direction = m_direction.norm();
}

DirectionalLightObject::~DirectionalLightObject()
{
}
