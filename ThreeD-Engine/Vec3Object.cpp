#include "Vec3Object.h"

Vec3Object::Vec3Object(std::string name, SceneSystem* system, vec3 value)
	: SceneObject(name, system), m_value(value)
{
}

Vec3Object::~Vec3Object()
{
}

ScriptValue* Vec3Object::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "value") return new Vec3ScriptValue(&m_value);
}
