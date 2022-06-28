#include "TerrainObject.h"
#include "AppWindow.h"
#include "Material.h"
#include "VertexMesh.h"
#include "Perlin.h"
#include "RWStructuredBuffer.h"
#include <tiny_obj_loader.h>
#include <map>

vec3 color(float height, float amplitude) {
	vec3 sand = vec3(201, 178, 99) / 255;
	vec3 grass = vec3(135, 184, 82) / 255;
	vec3 high_grass = vec3(80, 171, 93) / 255;
	vec3 mountain = vec3(120, 120, 120) / 255;
	vec3 snow = vec3(200, 200, 210) / 255;


	float norm = height / amplitude;
	if (norm < -0.1) {
		return sand;
	}
	if (norm < 0.1) {
		return vec3::lerp_bounds(sand, grass, -0.1, 0.1, norm);
	}
	if (norm < 0.2) {
		return vec3::lerp_bounds(grass, high_grass, 0.1, 0.2, norm);
	}
	if (norm < 0.4) {
		return high_grass;
	}
	if (norm < 0.6) {
		return mountain;
	}
	return vec3::lerp_bounds(mountain, snow, 0.6, 1, norm);
}

struct Triangle {
	vec3 v1, v2, v3;
};

float saturate(float f) {
	if (f < 0) return 0;
	if (f > 1) return 1;
	return f;
}

vec3 normal(Triangle& t) {
	return vec3::cross(t.v2 - t.v1, t.v3 - t.v1).normal();
}

struct compareVector {
	bool operator()(vec3 a, vec3 b) const {
		a.x = floor(a.x * 10) / 10;
		a.y = floor(a.y * 10) / 10;
		a.z = floor(a.z * 10) / 10;
		b.x = floor(b.x * 10) / 10;
		b.y = floor(b.y * 10) / 10;
		b.z = floor(b.z * 10) / 10;
		if (a.x != b.x) {
			return a.x < b.x;
		}
		if (a.y != b.y) {
			return a.y < b.y;
		}
		if (a.z != b.z) {
			return a.z < b.z;
		}
		return false;
	}
};

#define randomFloat() rand()/RAND_MAX

__declspec(align(16))
struct cbuf {
	vec3 start_position;
};

cbuf cb;

MeshPtr TerrainObject::createTerrainMesh(PerlinNoise p, int x, int z) {
	int height = chunk_height + 1;
	int size = chunk_size + 1;

	cb.start_position = vec3(x, 0, z) * chunk_size;

	RWStructuredBufferPtr pointsbuffer = GraphicsEngine::get()->getRenderSystem()->createAppendStructuredBuffer(sizeof(vec4), size * height * size);
	ConstantBufferPtr cbu = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cb, sizeof(cbuf));


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setComputeShader(pointshader);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRWStructuredBufferCS(pointsbuffer, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferCS(cbu, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->compute(size / 8 + 1, height / 8 + 1, size / 8 + 1);


	RWStructuredBufferPtr buf = GraphicsEngine::get()->getRenderSystem()->createAppendStructuredBuffer(sizeof(Triangle), size * height * size * 10);
	pointsbuffer->toCPU(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	vec4* points = (vec4*)pointsbuffer->open_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());

	StructuredBufferPtr pts = GraphicsEngine::get()->getRenderSystem()->createStructuredBuffer(points, sizeof(vec4), size * height * size);

	pointsbuffer->close_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setComputeShader(march);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setStructuredBufferCS(pts, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRWStructuredBufferCS(buf, 0);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->compute(chunk_size / 8, chunk_height / 8, chunk_size / 8);

	buf->toCPU(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	Triangle* tris = (Triangle*)buf->open_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());

	std::vector<TerrainMesh> list_vertices;
	std::vector<unsigned int> list_indices;
	std::vector<MaterialIndexRange> materials;

	std::map<vec3, int, compareVector> vertices;
	std::map<vec3, std::pair<vec3, int>, compareVector> normals;

	for (int i = 0; i < size * height * size * 10; i++) {
		if (tris->v1.mag() < 0.001 && tris->v2.mag() < 0.001 && tris->v3.mag() < 0.001) {
			break;
		}
		vertices.insert({ tris->v1, vertices.size() });
		vertices.insert({ tris->v2, vertices.size() });
		vertices.insert({ tris->v3, vertices.size() });
		normals[tris->v1].first += normal(*tris);
		normals[tris->v1].second++;
		normals[tris->v2].first += normal(*tris);
		normals[tris->v2].second++;
		normals[tris->v3].first += normal(*tris);
		normals[tris->v3].second++;
		list_indices.push_back(vertices[tris->v1]);
		list_indices.push_back(vertices[tris->v2]);
		list_indices.push_back(vertices[tris->v3]);

		tris++;
	}
	for (auto it = normals.begin(); it != normals.end(); ++it) {
		it->second.first.normalize();
	}
	list_vertices.resize(vertices.size());
	for (auto&& [vertex, id] : vertices) {
		list_vertices[id] = TerrainMesh(vertex, color(vertex.y, height / 2), normals[vertex].first);
	}
	buf->close_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());

	tinyobj::material_t* mat = new tinyobj::material_t();
	mat->ambient[0] = 0.2;
	mat->ambient[1] = 0.2;
	mat->ambient[2] = 0.2;
	mat->diffuse[0] = 1;
	mat->diffuse[1] = 1;
	mat->diffuse[2] = 1;
	mat->specular[0] = 0.2;
	mat->specular[1] = 0.2;
	mat->specular[2] = 0.2;
	mat->shininess = 1;
	mat->ambient_texname = "";
	mat->diffuse_texname = "";
	mat->specular_texname = "";
	materials.push_back({ 0, (int)list_indices.size(), std::make_shared<Material>(mat) });

	return std::make_shared<Mesh>(list_vertices, list_indices, materials);
}

TerrainObject::TerrainObject(std::string name, SceneSystem* system, int chunk_size, int chunk_height)
	: SceneObject(name, system), chunk_size(chunk_size), chunk_height(chunk_height)
{

	PerlinNoise p = PerlinNoise(2022, 9, 33 / 2, 0.4);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"TerrainDensityCompute.hlsl", "Density", &shader_byte_code, &size_shader);
	pointshader = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"MarchingCubes.hlsl", "March", &shader_byte_code, &size_shader);
	march = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

}

TerrainObject::~TerrainObject()
{
}

ScriptValue* TerrainObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "chunk_size") return new NumberScriptValue(&chunk_size);
	if (s == "chunk_height") return new NumberScriptValue(&chunk_height);
	return nullptr;
}
