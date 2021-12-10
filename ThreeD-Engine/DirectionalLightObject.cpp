#include "DirectionalLightObject.h"

DirectionalLightObject::DirectionalLightObject(std::string name, SceneSystem* system, vec3 color, vec3 direction)
	: SceneObject(name, system), m_color(color), m_direction(direction)
{
	m_direction = m_direction.norm();
}

DirectionalLightObject::~DirectionalLightObject()
{
}

ScriptValue* DirectionalLightObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "direction") return new Vec3ScriptValue(&m_direction);
	if (s == "color") return new Vec3ScriptValue(&m_color);
	return nullptr;
}
