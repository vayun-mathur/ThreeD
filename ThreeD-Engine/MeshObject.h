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

public:
	inline vec3 getPosition() { return m_position; }
	inline void setPosition(vec3 position) { m_position = position; }
	inline vec3 getScale() { return m_scale; }
	inline void setScale(vec3 scale) { m_scale = scale; }
private:
	virtual void render(ConstantBufferPtr cb);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	MeshPtr m_mesh;
private:
	vec3 m_position = vec3(0, 0, 0);
	vec3 m_scale = vec3(1, 1, 1);
};

