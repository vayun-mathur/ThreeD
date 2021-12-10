#include "PointLightObject.h"

PointLightObject::PointLightObject(std::string name, SceneSystem* system, vec3 color, vec3 position, float radius, vec3 attenuation)
	: SceneObject(name, system), m_color(color), m_position(position), m_radius(radius), m_attenuation(attenuation)
{
}

PointLightObject::~PointLightObject()
{
}
