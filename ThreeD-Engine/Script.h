#pragma once
#include <vector>
#include <string>

class SceneObject;

class Script
{
public:
	Script(std::vector<std::string> commands);
	~Script();

	void call(SceneObject* origin);

	std::vector<std::string> getCommands() { return m_commands; }
private:
	std::vector<std::string> m_commands;
};
