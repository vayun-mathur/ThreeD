#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class MeshObject
	: public SceneObject
{
public:
	MeshObject(std::string name, SceneSystem* system, std::wstring mesh_location);

	~MeshObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::MeshObject;
	}

	inline MeshPtr getMesh() { return m_mesh; }

	virtual ScriptValue* dot(std::string);
public:
	inline vec3 getPosition() { return m_position; }
	inline void setPosition(vec3 position) { m_position = position; }
	inline vec3 getScale() { return m_scale; }
	inline void setScale(vec3 scale) { m_scale = scale; }
private:
	MeshPtr m_mesh;
private:
	vec3 m_position = vec3(0, 0, 0);
	vec3 m_scale = vec3(1, 1, 1);
};

