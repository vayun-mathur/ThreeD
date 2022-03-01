#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class WaterTileObject
	: public SceneObject
{
public:
	WaterTileObject(std::string name, SceneSystem* system, VertexShaderPtr vs, PixelShaderPtr ps);

	~WaterTileObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::WaterTileObject;
	}

	inline VertexShaderPtr getVertexShader() { return m_vs; }
	inline PixelShaderPtr getPixelShader() { return m_ps; }
	inline static MeshPtr getMesh() { return m_mesh; }

	virtual ScriptValue* dot(std::string);
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
	static MeshPtr m_mesh;
	static TexturePtr m_dudv;
	static TexturePtr m_normal_map;
private:
	vec3 m_position = vec3(0, 0, 0);
	vec3 m_scale = vec3(1, 1, 1);
};

inline MeshPtr WaterTileObject::m_mesh = nullptr;
inline TexturePtr WaterTileObject::m_dudv = nullptr;
inline TexturePtr WaterTileObject::m_normal_map = nullptr;