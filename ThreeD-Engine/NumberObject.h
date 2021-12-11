#pragma once
#include "prerequisites.h"
#include "SceneObject.h"

class NumberObject
	: public SceneObject
{
public:
	NumberObject(std::string name, SceneSystem* system, float value);

	~NumberObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::NumberObject;
	}

	virtual ScriptValue* dot(std::string);
public:
	inline float getValue() { return m_value; }
	inline void setValue(float value) { m_value = value; }

private:
	float m_value;
};
