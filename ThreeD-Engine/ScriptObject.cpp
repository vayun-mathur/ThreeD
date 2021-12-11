#include "ScriptObject.h"

ScriptObject::ScriptObject(std::string name, SceneSystem* system, Script script)
	: SceneObject(name, system), m_script(script)
{
}

ScriptObject::~ScriptObject()
{
}

ScriptValue* ScriptObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	return nullptr;
}

void ScriptObject::call()
{
	m_script.call(this);
}
