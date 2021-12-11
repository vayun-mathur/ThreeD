#pragma once
#include <vector>
#include <string>
#include "ScriptSystem.h"

class SceneObject;

class Script
{
public:
	Script(std::string commands);
	Script();
	~Script();

	void call(SceneObject* origin);

	std::string getCommands() { return m_commands; }
private:
	std::string m_commands;
};

class CodeValue : public ScriptValue {
public:
	Script* v;
	CodeValue(Script* v) : v(v) {}
	virtual ScriptValue* dot(std::string) { return nullptr; }
	virtual ScriptValue* add(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* sub(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* mul(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* div(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* addassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* subassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* mulassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* divassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* assign(ScriptValue* o) {
		if (instanceof<CodeValue>(o)) {
			*v = *((CodeValue*)o)->v;
			return this;
		}
		return nullptr;
	}
};