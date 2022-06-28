#pragma once


#include <memory>
#include <vector>
#include "Mesh2.h"

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
	TetrahedralMesh();
	~TetrahedralMesh();

	Mesh GetMesh();

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

