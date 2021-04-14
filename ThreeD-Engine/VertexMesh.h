#pragma once
#include "Vector.h"

class VertexMesh {
public:
	VertexMesh() : m_position(), m_texcoord() {}
	VertexMesh(vec3 position, vec2 texcoord) : m_position(position), m_texcoord(texcoord) {}
	VertexMesh(const VertexMesh& vertex) : m_position(vertex.m_position), m_texcoord(vertex.m_texcoord) {}

	~VertexMesh() {

	}
public:
	vec3 m_position;
	vec2 m_texcoord;
};