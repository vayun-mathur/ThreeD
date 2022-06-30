#include "PhysicsSystem.h"
#include "PhysicalObject.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "TetrahedralMesh.h"
#include "EditableMesh.h"
#include "FEA.h"

#include <chrono>
#include <iostream>

__declspec(align(16))
struct cbufC
{
	float ks = 10, kd = 0.5;
	float rest_length = 0.1;
	float dt = 0.0002;
	uint32_t size = 20;
};

cbufC buf;

std::shared_ptr<FEA> m_fea;
std::shared_ptr<TetrahedralMesh> m_tetrahedral_mesh;

PhysicsSystem::PhysicsSystem()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"SoftBody.hlsl", "UpdateSprings", &shader_byte_code, &size_shader);
	soft_body = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	cbuf = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(cbufC));

	m_tetrahedral_mesh = std::make_shared<TetrahedralMesh>();
	m_fea = std::make_shared<FEA>(m_tetrahedral_mesh, 0.1, 0.0001);

	auto force_node_index = (8 * 10) + (4 * 2) + 0;
	m_fea->SetForce(force_node_index, vec3(0.002f, -0.04f, 0.08f));
}

void PhysicsSystem::update(std::vector<PhysicalObjectPtr>& objects, double dt)
{
	this->physicals = objects;

	for (int i = 0; i < 10; i++) {
		m_fea->Update();
	}
	auto new_tetrahedral_mesh = m_tetrahedral_mesh->GetMesh();

	EditableMeshPtr ptr = physicals[0]->getMesh();
	std::vector<Triangle>& tris = ptr->getTriangles();
	tris.clear();
	for (int i = 0; i < new_tetrahedral_mesh.m_indices.size();i+=3) {
		tris.push_back(Triangle{ new_tetrahedral_mesh.m_vertex_positions[new_tetrahedral_mesh.m_indices[i]],
			new_tetrahedral_mesh.m_vertex_positions[new_tetrahedral_mesh.m_indices[i+1]] ,
			new_tetrahedral_mesh.m_vertex_positions[new_tetrahedral_mesh.m_indices[i+2]] });
	}
	ptr->updateMesh();
}
