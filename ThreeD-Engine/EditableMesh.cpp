#include "EditableMesh.h"

#include <tiny_obj_loader.h>

#include <iostream>
#include "GraphicsEngine.h"
#include "VertexBuffer.h"

EditableMesh::EditableMesh()
{
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
	mat->ambient_texname = "";
	mat->diffuse_texname = "";
	mat->specular_texname = "";
	m_material_indices.push_back({ 0, 588*3, std::make_shared<Material>(mat) });

	VertexMesh list_vertices[588*3];

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 0);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(VertexMesh),
		588*3, shader_byte_code, size_shader, 0);
}

const VertexBufferPtr& EditableMesh::getVertexBuffer()
{
	return m_vertex_buffer;
}

const std::vector<MIR> EditableMesh::getMaterialIterable()
{
	return m_material_indices;
}

std::vector<Triangle>& EditableMesh::getTriangles()
{
	return m_triangles;
}

void EditableMesh::updateMesh()
{
	std::vector<VertexMesh> list_vertices;
	for (Triangle& t : m_triangles) {
		vec3 normal = vec3::cross(t.p3 - t.p1, t.p2 - t.p1);
		list_vertices.push_back(VertexMesh(t.p1, 0, normal));
		list_vertices.push_back(VertexMesh(t.p2, 0, normal));
		list_vertices.push_back(VertexMesh(t.p3, 0, normal));
	}
	m_material_indices[0].low = 0;
	m_material_indices[0].high = list_vertices.size();
	m_vertex_buffer->editBuffer(&list_vertices[0], sizeof(VertexMesh), list_vertices.size());
}
