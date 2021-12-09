#include "DirectionalLightObject.h"

DirectionalLightObject::DirectionalLightObject(std::string name, vec3 color, vec3 direction)
	: SceneObject(name), m_color(color), m_direction(direction)
{
	m_direction = m_direction.norm();
}

DirectionalLightObject::~DirectionalLightObject()
{
}
