#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class WaterTileObject
	: public SceneObject
{
public:
	WaterTileObject(std::string name, SceneSystem* system);

	~WaterTileObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::WaterTileObject;
	}

	virtual ScriptValue* dot(std::string);
public:
	inline TexturePtr getDUDV() { return m_dudv; }
	inline TexturePtr getNormalMap() { return m_normal_map; }
public:
	inline vec3 getPosition() { return m_position; }
	inline void setPosition(vec3 position) { m_position = position; }
	inline vec3 getScale() { return m_scale; }
	inline void setScale(vec3 scale) { m_scale = scale; }
private:
	TexturePtr m_dudv;
	TexturePtr m_normal_map;
private:
	vec3 m_position = vec3(0, 0, 0);
	vec3 m_scale = vec3(1, 1, 1);
};