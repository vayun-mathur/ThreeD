#include "PhysicalObject.h"
#include "AppWindow.h"
#include "EditableMesh.h"
#include "TetrahedralMesh.h"

#include "FEA.h"

PhysicalObject::PhysicalObject(std::string name, SceneSystem* system)
	: SceneObject(name, system)
{
	m_mesh = std::make_shared<EditableMesh>();
	m_tetrahedral = std::make_shared<TetrahedralMesh>(8, 4, 1, 0.75);
	m_fea = std::make_shared<FEA>(m_tetrahedral, 0.1, 0.0001);
}

PhysicalObject::~PhysicalObject()
{
}

ScriptValue* PhysicalObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	return nullptr;
}