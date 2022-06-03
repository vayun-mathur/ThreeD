#include "PhysicalObject.h"
#include "AppWindow.h"
#include "EditableMesh.h"

PhysicalObject::PhysicalObject(std::string name, SceneSystem* system, std::wstring mesh_location)
	: SceneObject(name, system)
{
	m_mesh = std::make_shared<EditableMesh>(mesh_location.c_str());
}

PhysicalObject::~PhysicalObject()
{
}

ScriptValue* PhysicalObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	return nullptr;
}