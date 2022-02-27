#pragma once
#include "Vector.h"

class VertexMesh {
public:
	VertexMesh() : m_position(), m_texcoord(), m_normal() {}
	VertexMesh(vec3 position, vec2 texcoord, vec3 normal) : m_position(position), m_texcoord(texcoord), m_normal(normal) {}
	VertexMesh(const VertexMesh& vertex) : m_position(vertex.m_position), m_texcoord(vertex.m_texcoord), m_normal(vertex.m_normal) {}

	~VertexMesh() {

	}
public:
	vec3 m_position;
	vec2 m_texcoord;
	vec3 m_normal;
};

class TerrainMesh {
public:
	TerrainMesh() : m_position(), m_color(), m_normal() {}
	TerrainMesh(vec3 position, vec3 color, vec3 normal) : m_position(position), m_color(color), m_normal(normal) {}
	TerrainMesh(const TerrainMesh& vertex) : m_position(vertex.m_position), m_color(vertex.m_color), m_normal(vertex.m_normal) {}

	~TerrainMesh() {

	}
public:
	vec3 m_position;
	vec3 m_color;
	vec3 m_normal;
};

class WaterMesh {
public:
	WaterMesh() : m_position() {}
	WaterMesh(vec2 position) : m_position(position) {}
	WaterMesh(const WaterMesh& vertex) : m_position(vertex.m_position) {}

	~WaterMesh() {

	}
public:
	vec2 m_position;
};