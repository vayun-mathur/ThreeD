#include "NumberObject.h"

NumberObject::NumberObject(std::string name, SceneSystem* system, float value) 
	: SceneObject(name, system), m_value(value)
{
}

NumberObject::~NumberObject()
{
}

ScriptValue* NumberObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "value") return new NumberScriptValue(&m_value);
}
