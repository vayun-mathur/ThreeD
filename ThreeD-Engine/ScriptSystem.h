#pragma once
#include "prerequisites.h"
#include <string>
#include <map>
#include "Vector.h"

class ScriptValue {
public:
	virtual bool bool_value() { return false; };
	virtual ScriptValue* dot(std::string) = 0;
	virtual ScriptValue* add(ScriptValue*) = 0;
	virtual ScriptValue* sub(ScriptValue*) = 0;
	virtual ScriptValue* mul(ScriptValue*) = 0;
	virtual ScriptValue* div(ScriptValue*) = 0;
	virtual ScriptValue* addassign(ScriptValue*) = 0;
	virtual ScriptValue* subassign(ScriptValue*) = 0;
	virtual ScriptValue* mulassign(ScriptValue*) = 0;
	virtual ScriptValue* divassign(ScriptValue*) = 0;
	virtual ScriptValue* assign(ScriptValue*) = 0;
};

class NumberScriptValue : public ScriptValue {
public:
	float* v;
	NumberScriptValue(float* v) : v(v) {}
	virtual bool bool_value() { return *v != 0; }
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
	virtual ScriptValue* addassign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v += *((NumberScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
	virtual ScriptValue* subassign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v -= *((NumberScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
	virtual ScriptValue* mulassign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v *= *((NumberScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
	virtual ScriptValue* divassign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v /= *((NumberScriptValue*)o)->v;
			return this;
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
	virtual bool bool_value() { return !(v->x==0 && v->y==0 && v->z==0); }
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
	virtual ScriptValue* addassign(ScriptValue* o) {
		if (instanceof<Vec3ScriptValue>(o)) {
			*v = *v + *((Vec3ScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
	virtual ScriptValue* subassign(ScriptValue* o) {
		if (instanceof<Vec3ScriptValue>(o)) {
			*v = *v - *((Vec3ScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
	virtual ScriptValue* mulassign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v = *v * *((NumberScriptValue*)o)->v;
			return this;
		}
		return nullptr;
	}
	virtual ScriptValue* divassign(ScriptValue* o) {
		if (instanceof<NumberScriptValue>(o)) {
			*v = *v * (1 / *((NumberScriptValue*)o)->v);
			return this;
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

	void exec(std::string cmd, SceneObject* object, std::map<std::string, ScriptValue*>& var_in);
public:
	static ScriptSystem* get();
	static void create();
	static void release();
private:
	static ScriptSystem* m_system;
};

