#include "TerrainObject.h"
#include "AppWindow.h"
#include "Material.h"
#include "VertexMesh.h"
#include "Perlin.h"
#include <tiny_obj_loader.h>

vec3 calcNormal(vec3 vertex0, vec3 vertex1, vec3 vertex2) {
	vec3 tangentA = vertex1 - vertex0;
	vec3 tangentB = vertex2 - vertex0;
	vec3 normal = vec3::cross(tangentA, tangentB);
	normal.normalize();
	return normal;
}

void storeTriangle(vec3* cornerPos, vec3 normal, vec3* colours, std::vector<TerrainMesh>& list_vertices,
	int index0, int index1, int index2) {
	list_vertices.push_back(TerrainMesh(cornerPos[index0], colours[index0], normal));
	list_vertices.push_back(TerrainMesh(cornerPos[index1], colours[index1], normal));
	list_vertices.push_back(TerrainMesh(cornerPos[index2], colours[index2], normal));
}

vec3* calculateCornerPositions(int col, int row, float** heights) {
	vec3* vertices = new vec3[4];
	vertices[0] = vec3(col, heights[row][col], row);
	vertices[1] = vec3(col, heights[row + 1][col], row + 1);
	vertices[2] = vec3(col + 1, heights[row][col + 1], row);
	vertices[3] = vec3(col + 1, heights[row + 1][col + 1], row + 1);
	return vertices;
}

vec3* calculateCornerColours(int col, int row, vec3** colours) {
	vec3* cornerCols = new vec3[4];
	cornerCols[0] = colours[row][col];
	cornerCols[1] = colours[row + 1][col];
	cornerCols[2] = colours[row][col + 1];
	cornerCols[3] = colours[row + 1][col + 1];
	return cornerCols;
}

void storeGridSquare(int col, int row, float** heights, vec3** colours, std::vector<TerrainMesh>& list_vertices) {
	vec3* cornerPos = calculateCornerPositions(col, row, heights);
	vec3* cornerCols = calculateCornerColours(col, row, colours);
	vec3 normalTopLeft = calcNormal(cornerPos[0], cornerPos[1], cornerPos[2]);
	vec3 normalBottomRight = calcNormal(cornerPos[2], cornerPos[1], cornerPos[3]);
	storeTriangle(cornerPos, normalTopLeft, cornerCols, list_vertices, 0, 1, 2);
	storeTriangle(cornerPos, normalBottomRight, cornerCols, list_vertices, 2, 1, 3);
}


MeshPtr createTerrain(float** heights, vec3** colours, int gridSize) {
	std::vector<TerrainMesh> list_vertices;
	std::vector<unsigned int> list_indices;
	std::vector<MaterialIndexRange> materials;

	for (int i = 0; i < gridSize * gridSize * 6; i++) {
		list_indices.push_back(i);
	}

	for (int row = 0; row < gridSize; row++) {
		for (int col = 0; col < gridSize; col++) {
			storeGridSquare(col, row, heights, colours, list_vertices);
		}
	}

	tinyobj::material_t* mat = new tinyobj::material_t();
	mat->ambient[0] = 1;
	mat->ambient[1] = 1;
	mat->ambient[2] = 1;
	mat->diffuse[0] = 0.2;
	mat->diffuse[1] = 0.2;
	mat->diffuse[2] = 0.2;
	mat->specular[0] = 0;
	mat->specular[1] = 0;
	mat->specular[2] = 0;
	mat->shininess = 1;
	mat->ambient_texname = "terrain.png";
	mat->diffuse_texname = "";
	mat->specular_texname = "";
	materials.push_back({ 0, (int)list_indices.size(), std::make_shared<Material>(mat) });

	return std::make_shared<Mesh>(list_vertices, list_indices, materials);
}

float** generateHeights(int gridSize, PerlinNoise perlinNoise) {
	float** heights = new float* [gridSize + 1];
	for (int i = 0; i <= gridSize; i++) {
		heights[i] = new float[gridSize + 1];
	}
	for (int z = 0; z <= gridSize; z++) {
		for (int x = 0; x <= gridSize; x++) {
			heights[z][x] = perlinNoise.getPerlinNoise(x, z);
		}
	}
	return heights;
}

MeshPtr generateTerrain(int gridSize, PerlinNoise perlinNoise) {
	float** heights = generateHeights(gridSize, perlinNoise);
	vec3** colours = ColorGenerator(new vec3[]{ vec3(201, 178, 99) * (1.f / 255),
			vec3(135, 184, 82) * (1.f / 255), vec3(80, 171, 93) * (1.f / 255), vec3(120, 120, 120) * (1.f / 255),
			vec3(200, 200, 210) * (1.f / 255) }, 5, 0.45).generateColours(heights, gridSize, perlinNoise.getAmplitude());
	return createTerrain(heights, colours, gridSize);
}

TerrainObject::TerrainObject(std::string name, SceneSystem* system)
	: SceneObject(name, system)
{
	m_mesh = generateTerrain(400, PerlinNoise(3, 10, 0.05));
}

TerrainObject::~TerrainObject()
{
}

ScriptValue* TerrainObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "position") return new Vec3ScriptValue(&m_position);
	if (s == "scale") return new Vec3ScriptValue(&m_scale);
	return nullptr;
}
