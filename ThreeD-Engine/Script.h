#pragma once
#include <vector>
#include <string>
#include <map>
#include "ScriptSystem.h"

class SceneObject;

class Script
{
public:
	Script(std::string commands);
	Script();
	~Script();

	void call(SceneObject* origin, std::map<std::string, ScriptValue*> var_in);

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
	virtual ScriptValue* equal_to(ScriptValue* o) { return new BoolScriptValue(new bool(this == o)); }
	virtual ScriptValue* not_equal_to(ScriptValue* o) { return new BoolScriptValue(new bool(this != o)); }
	virtual ScriptValue* greater_than(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* greater_than_or_equal_to(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* less_than(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* less_than_or_equal_to(ScriptValue* o) { return nullptr; }
};