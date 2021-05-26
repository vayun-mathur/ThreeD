#include "PointLightObject.h"

PointLightObject::PointLightObject(std::string name, vec3 color, vec3 position, float radius)
	: SceneObject(name), m_color(color), m_position(position), m_radius(radius)
{
}

PointLightObject::~PointLightObject()
{
}
