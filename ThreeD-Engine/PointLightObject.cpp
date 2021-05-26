#include "PointLightObject.h"

PointLightObject::PointLightObject(std::string name, vec3 color, vec3 position)
	: SceneObject(name), m_color(color), m_position(position)
{
}

PointLightObject::~PointLightObject()
{
}
