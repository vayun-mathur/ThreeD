#include "DirectionalLightObject.h"

DirectionalLightObject::DirectionalLightObject(std::string name, vec3 direction)
	: SceneObject(name), m_direction(direction)
{
}

DirectionalLightObject::~DirectionalLightObject()
{
}
