#pragma once
#include "Resource.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "prerequisites.h"
#include "VertexMesh.h"
#include <vector>

struct MaterialIndexRange {
	int low;
	int high;
	MaterialPtr material;
};

class Mesh : public Resource
{
public:
	Mesh(const wchar_t* full_path);
	Mesh(std::vector<VertexMesh> list_vertices, std::vector<unsigned int> list_indices, std::vector<MaterialIndexRange> materials);
	~Mesh();
	const VertexBufferPtr& getVertexBuffer();
	const IndexBufferPtr& getIndexBuffer();
	const std::vector<MaterialIndexRange> getMaterials();
private:
	VertexBufferPtr m_vertex_buffer;
	IndexBufferPtr m_index_buffer;
	std::vector<MaterialIndexRange> m_materials;
private:
	friend class DeviceContext;
};
