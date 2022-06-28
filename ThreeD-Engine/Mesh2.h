#pragma once

#include "Vector.h"
#include <vector>

struct Mesh
{
public:
	Mesh(std::vector<vec3> vertex_positions,
		std::vector<vec3> vertex_colors,
		std::vector<vec3> vertex_normals,
		std::vector<short> indices)
	{
		m_vertex_positions = vertex_positions;
		m_vertex_colors = vertex_colors;
		m_vertex_normals = vertex_normals;
		m_indices = indices;
	}

	std::vector<vec3> m_vertex_positions;
	std::vector<vec3> m_vertex_colors;
	std::vector<vec3> m_vertex_normals;
	std::vector<short> m_indices;
};

