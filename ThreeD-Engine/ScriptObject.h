#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include <vector>

class ScriptObject
	: public SceneObject
{
public:
	ScriptObject(std::string name, SceneSystem* system, std::vector<std::string> commands);

	~ScriptObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::ScriptObject;
	}

	virtual ScriptValue* dot(std::string);

	void call();

	virtual void init() { call(); }
	virtual void update(double delta_time) {}
private:
	std::vector<std::string> m_commands;
};

