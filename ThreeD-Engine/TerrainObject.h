#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
#include "Perlin.h"
#include <unordered_map>

typedef std::pair<int, int> c2;

class TerrainObject
	: public SceneObject
{
public:
	TerrainObject(std::string name, SceneSystem* system, int chunk_size, int chunk_height);

	~TerrainObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::TerrainObject;
	}
	inline const std::map<c2, MeshPtr>& getMeshes() { return m_meshes; }

	const std::vector<MeshPtr> getMeshes(vec3 pos, bool generate);

	virtual ScriptValue* dot(std::string);
private:
	MeshPtr createTerrainMesh(PerlinNoise p, int x, int z);
private:
	std::map<c2, MeshPtr> m_meshes;
	ComputeShaderPtr pointshader, march;
	float chunk_size = 64;
	float chunk_height = 32;
	PerlinNoise perlin;
};

