#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
class MeshObject
	: public SceneObject
{
public:
	MeshObject(std::string name, std::wstring mesh_location, std::wstring tex_location);

	~MeshObject();
private:
	virtual void render(ConstantBufferPtr cb);

private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	TexturePtr m_tex;
	MeshPtr m_mesh;
};

