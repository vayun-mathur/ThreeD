#pragma once
#include "prerequisites.h"
#include "VertexMesh.h"
#include "Material.h"
#include <vector>

struct MIR {
	int low;
	int high;
	MaterialPtr material;
};

struct Triangle {
	vec3 p1;
	vec3 p2;
	vec3 p3;
};

class EditableMesh {
public:
	EditableMesh();
	const VertexBufferPtr& getVertexBuffer();
	const std::vector<MIR> getMaterialIterable();

	std::vector<Triangle>& getTriangles();

	void updateMesh();

private:
	VertexBufferPtr m_vertex_buffer;
	std::vector<MIR> m_material_indices;

	std::vector<Triangle> m_triangles;
};