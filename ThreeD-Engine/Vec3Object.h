#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"

class Vec3Object
	: public SceneObject
{
public:
	Vec3Object(std::string name, SceneSystem* system, vec3 value);

	~Vec3Object();

	virtual SceneObjectType getType() const {
		return SceneObjectType::NumberObject;
	}

	virtual ScriptValue* dot(std::string);
public:
	inline vec3 getValue() { return m_value; }
	inline void setValue(vec3 value) { m_value = value; }

private:
	vec3 m_value;
};
