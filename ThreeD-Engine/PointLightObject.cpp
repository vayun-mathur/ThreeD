#include "PointLightObject.h"

PointLightObject::PointLightObject(std::string name, SceneSystem* system, vec3 color, vec3 position, float radius, vec3 attenuation)
	: SceneObject(name, system), m_color(color), m_position(position), m_radius(radius), m_attenuation(attenuation)
{
}

PointLightObject::~PointLightObject()
{
}

ScriptValue* PointLightObject::dot(std::string s)
{
	if (s == "position") return new Vec3ScriptValue(&m_position);
	if (s == "color") return new Vec3ScriptValue(&m_color);
	if (s == "attenuation") return new Vec3ScriptValue(&m_attenuation);
	if (s == "radius") return new NumberScriptValue(&m_radius);
	return nullptr;
}
