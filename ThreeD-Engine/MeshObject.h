#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class MeshObject
	: public SceneObject
{
public:
	MeshObject(std::string name, std::wstring mesh_location, std::wstring material_location);

	~MeshObject();
private:
	virtual void render(ConstantBufferPtr cb);

private:
	MaterialPtr m_material;
	MeshPtr m_mesh;
public:
	vec3 position = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);
};

