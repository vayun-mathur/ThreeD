#pragma once
#include "prerequisites.h"
#include <string>
#include "Vector.h"

class ScriptValue {
public:
	virtual ScriptValue* dot(std::string) = 0;
	virtual ScriptValue* add(ScriptValue*) = 0;
	virtual ScriptValue* sub(ScriptValue*) = 0;
	virtual ScriptValue* mul(ScriptValue*) = 0;
	virtual ScriptValue* div(ScriptValue*) = 0;
	virtual ScriptValue* assign(ScriptValue*) = 0;
};

class NumberScriptValue : public ScriptValue {
public:
	float* v;
	NumberScriptValue(float* v) : v(v) {}
	virtual ScriptValue* dot(std::string) { return nullptr; }
	virtual ScriptValue* add(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			return new NumberScriptValue(new float(*v + *((NumberScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* sub(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			return new NumberScriptValue(new float(*v - *((NumberScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* mul(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			return new NumberScriptValue(new float(*v * *((NumberScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* div(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			return new NumberScriptValue(new float(*v / *((NumberScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* assign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v = *((NumberScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
};

class Vec3ScriptValue : public ScriptValue {
public:
	vec3* v;
	Vec3ScriptValue(vec3* v) : v(v) {}
	virtual ScriptValue* dot(std::string s) {
		if (s == "x") return new NumberScriptValue(&v->x);
		if (s == "y") return new NumberScriptValue(&v->y);
		if (s == "z") return new NumberScriptValue(&v->z);
		return nullptr;
	}
	virtual ScriptValue* add(ScriptValue* o) {
		if (instanceof<Vec3ScriptValue>(o)) {
			return new Vec3ScriptValue(new vec3(*v + *((Vec3ScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* sub(ScriptValue* o) {
		if (instanceof<Vec3ScriptValue>(o)) {
			return new Vec3ScriptValue(new vec3(*v - *((Vec3ScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* mul(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			return new Vec3ScriptValue(new vec3(*v * *((NumberScriptValue*)o)->v));
		}
		return nullptr;
	}
	virtual ScriptValue* div(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			return new Vec3ScriptValue(new vec3(*v * (1/ *((NumberScriptValue*)o)->v)));
		}
		return nullptr;
	}
	virtual ScriptValue* assign(ScriptValue* o) {
		if (instanceof<Vec3ScriptValue>(o)) {
			*v = *((Vec3ScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
};

class ScriptSystem
{
public:
	ScriptSystem();
	~ScriptSystem();

	void exec(std::string cmd, SceneObject* object);
public:
	static ScriptSystem* get();
	static void create();
	static void release();
private:
	static ScriptSystem* m_system;
};

