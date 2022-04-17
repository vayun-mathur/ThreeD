#include "Script.h"
#include "ScriptSystem.h"

Script::Script(std::string commands) : m_commands(commands)
{
}
Script::Script() : m_commands("")
{
}

Script::~Script()
{
}

ScriptValue* Script::call(SceneObject* origin, std::map<std::string, ScriptValue*>& var_in)
{
	if (m_commands == "") return nullptr;
	return ScriptSystem::get()->exec(m_commands, origin, var_in);
}
