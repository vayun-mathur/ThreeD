#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>

#include "GraphicsEngine.h"
#include "VertexMesh.h"

Mesh::Mesh(const wchar_t* full_path) : Resource(full_path)
{

	std::string warn;
	std::string err;

	tinyobj::ObjReaderConfig reader_config;
	std::wstring input = full_path;
	std::string input_path = std::string(input.begin(), input.end());
	reader_config.mtl_search_path = input_path.substr(0, input_path.find_last_of('\\') + 1); // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(input_path, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		throw std::exception("Mesh not created successfully");
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}
	auto& attribs = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	if (!err.empty()) throw std::exception("Mesh not created successfully");

	//if(shapes.size() > 1) throw std::exception("Mesh not created successfully");

	std::vector<MaterialPtr> materials_lit;
	for (tinyobj::material_t mat : materials) {
		materials_lit.push_back(std::make_shared<Material>(&mat));
	}

	std::vector<VertexMesh> list_vertices;
	std::vector<unsigned int> list_indices;

	int low = -1;

	int material = -1;

	size_t index_offset_overall = 0;
	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			unsigned char num_face_verts = shapes[s].mesh.num_face_vertices[f];
			int mat = shapes[s].mesh.material_ids[f];
			if (mat != material) {
				if (material != -1)
					m_materials.push_back({ low, (int)list_indices.size(), materials_lit[material] });
				low = list_indices.size();
				material = mat;
			}

			for (unsigned char v = 0; v < num_face_verts; v++) {
				tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx, vy, vz;
				tinyobj::real_t tx, ty;
				tinyobj::real_t nx, ny, nz;
				vx = attribs.vertices[index.vertex_index * 3 + 0];
				vy = attribs.vertices[index.vertex_index * 3 + 1];
				vz = attribs.vertices[index.vertex_index * 3 + 2];

				if (index.texcoord_index != -1) {
					tx = attribs.texcoords[index.texcoord_index * 2 + 0];
					ty = attribs.texcoords[index.texcoord_index * 2 + 1];
				}
				else {
					tx = 0;
					ty = 0;
				}

				nx = attribs.normals[index.normal_index * 3 + 0];
				ny = attribs.normals[index.normal_index * 3 + 1];
				nz = attribs.normals[index.normal_index * 3 + 2];

				VertexMesh vertex(vec3(vx, vy, vz), vec2(tx, ty), vec3(nx, ny, nz));

				list_vertices.push_back(vertex);
				list_indices.push_back(index_offset_overall + index_offset + v);
			}

			index_offset += num_face_verts;
		}
		index_offset_overall += index_offset;
	}
	m_materials.push_back({ low, (int)list_indices.size(), materials_lit[material] });
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 0);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(VertexMesh),
		list_vertices.size(), shader_byte_code, size_shader, 0);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&list_indices[0], (UINT)list_indices.size());
}

Mesh::Mesh(std::vector<TerrainMesh> list_vertices, std::vector<unsigned int> list_indices, std::vector<MaterialIndexRange> materials)
	: Resource(L"")
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 1);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(TerrainMesh),
		list_vertices.size(), shader_byte_code, size_shader, 1);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&list_indices[0], (UINT)list_indices.size());
	m_materials = materials;
}

Mesh::~Mesh()
{
}

const VertexBufferPtr& Mesh::getVertexBuffer()
{
	return m_vertex_buffer;
}

const IndexBufferPtr& Mesh::getIndexBuffer()
{
	return m_index_buffer;
}

const std::vector<MaterialIndexRange> Mesh::getMaterials()
{
	return m_materials;
}
