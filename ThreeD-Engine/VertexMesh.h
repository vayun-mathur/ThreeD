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