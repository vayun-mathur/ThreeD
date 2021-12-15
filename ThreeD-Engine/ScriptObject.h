#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Script.h"
#include <vector>

class ScriptObject
	: public SceneObject
{
public:
	ScriptObject(std::string name, SceneSystem* system, Script commands);

	~ScriptObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::ScriptObject;
	}

	virtual ScriptValue* dot(std::string);

	void call(std::map<std::string, ScriptValue*> var_in);

	Script* getScript() { return &m_script; }

	virtual void init() { call({}); }
	virtual void update(double delta_time) {}
private:
	Script m_script;
};

