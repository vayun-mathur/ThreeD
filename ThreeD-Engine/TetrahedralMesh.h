#pragma once


#include <memory>
#include <vector>
#include "Vector.h"

struct Mesh2
{
public:
	Mesh2(std::vector<vec3> vertex_positions,
		std::vector<short> indices)
	{
		m_vertex_positions = vertex_positions;
		m_indices = indices;
	}

	std::vector<vec3> m_vertex_positions;
	std::vector<short> m_indices;
};



struct TetrahedralElementIndices
{
public:
	TetrahedralElementIndices(int inA, int inB, int inC, int inD)
	{
		a = inA;
		b = inB;
		c = inC;
		d = inD;
	}

	int a;
	int b;
	int c;
	int d;
};


class TetrahedralMesh
{
public:
	TetrahedralMesh(int x_num, int y_num, int z_num, float cube_size);
	~TetrahedralMesh();

	Mesh2 GetMesh();

	std::vector<vec3> GetNodes() { return m_nodes; }
	std::vector<TetrahedralElementIndices> GetElements() { return m_elements; }
	std::vector<int> GetFixedNodes() { return m_fixed_nodes; }

	void SetNodePosition(int index, vec3 position);

private:
	std::vector<TetrahedralElementIndices> GetTriangularPrismFromNodes(std::vector<int> node_indices);

	std::vector<int> m_fixed_nodes;
	std::vector<vec3> m_nodes;
	std::vector<TetrahedralElementIndices> m_elements;
};

