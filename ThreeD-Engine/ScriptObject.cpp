#include "ScriptObject.h"

ScriptObject::ScriptObject(std::string name, SceneSystem* system, std::vector<std::string> commands)
	: SceneObject(name, system), m_commands(commands)
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
	for (std::string command : m_commands) {
		ScriptSystem::get()->exec(command, this);
	}
}
