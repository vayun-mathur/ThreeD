#include "Script.h"
#include "ScriptSystem.h"

Script::Script(std::vector<std::string> commands) : m_commands(commands)
{
}

Script::~Script()
{
}

void Script::call(SceneObject* origin)
{
	for (std::string command : m_commands) {
		ScriptSystem::get()->exec(command, origin);
	}
}
