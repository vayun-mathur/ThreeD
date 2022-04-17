#include "MeshObject.h"
#include "AppWindow.h"
#include "Material.h"

MeshObject::MeshObject(std::string name, SceneSystem* system, std::wstring mesh_location, VertexShaderPtr vs, PixelShaderPtr ps)
	: SceneObject(name, system), m_vs(vs), m_ps(ps)
{
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(mesh_location.c_str());
}

MeshObject::~MeshObject()
{
}

ScriptValue* MeshObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "position") return new Vec3ScriptValue(&m_position);
	if (s == "scale") return new Vec3ScriptValue(&m_scale);
	return nullptr;
}