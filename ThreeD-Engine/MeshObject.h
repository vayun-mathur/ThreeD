#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class MeshObject
	: public SceneObject
{
public:
	MeshObject(std::string name, std::wstring mesh_location, VertexShaderPtr vs, PixelShaderPtr ps);

	~MeshObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::MeshObject;
	}

	inline VertexShaderPtr getVertexShader() { return m_vs; }
	inline PixelShaderPtr getPixelShader() { return m_ps; }
	inline MeshPtr getMesh() { return m_mesh; }
private:
	virtual void render(ConstantBufferPtr cb);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	MeshPtr m_mesh;
public:
	vec3 position = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);
};

