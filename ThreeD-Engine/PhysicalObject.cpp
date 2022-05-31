#include "PhysicalObject.h"
#include "AppWindow.h"
#include "Material.h"

PhysicalObject::PhysicalObject(std::string name, SceneSystem* system, std::wstring mesh_location, float mass)
	: SceneObject(name, system), lin_inertia(mass)
{
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(mesh_location.c_str());
}

PhysicalObject::~PhysicalObject()
{
}

ScriptValue* PhysicalObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "position") return new Vec3ScriptValue(&m_lin_position);
	return nullptr;
}