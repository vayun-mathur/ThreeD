#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class MeshObject
	: public SceneObject
{
public:
	MeshObject(std::string name, std::wstring mesh_location, std::wstring tex_location, VertexShaderPtr vs, PixelShaderPtr ps, bool cull_front);

	~MeshObject();
private:
	virtual void render(ConstantBufferPtr cb);

private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	TexturePtr m_tex;
	MeshPtr m_mesh;
	bool m_front_cull;
public:
	vec3 position = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);
};

