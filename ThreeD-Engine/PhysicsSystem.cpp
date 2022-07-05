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

PhysicsSystem::PhysicsSystem()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"SoftBody.hlsl", "UpdateSprings", &shader_byte_code, &size_shader);
	soft_body = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	cbuf = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(cbufC));

}

void PhysicsSystem::update(std::vector<PhysicalObjectPtr>& objects, double dt)
{
	this->physicals = objects;

	auto force_node_index = (8 * 10) + (4 * 2) + 0;
	physicals[0]->getFEA()->SetForce(force_node_index, vec3(0.002f, -0.04f, 0.08f));

	for (PhysicalObjectPtr physical : objects) {

		for (int i = 0; i < 10; i++) {
			physical->getFEA()->Update();
		}

		auto new_tetrahedral_mesh = physical->getTetrahedral()->GetMesh();

		EditableMeshPtr ptr = physical->getMesh();
		std::vector<Triangle>& tris = ptr->getTriangles();
		tris.clear();
		for (int i = 0; i < new_tetrahedral_mesh.m_indices.size(); i += 3) {
			tris.push_back(Triangle{ new_tetrahedral_mesh.m_vertex_positions[new_tetrahedral_mesh.m_indices[i]],
				new_tetrahedral_mesh.m_vertex_positions[new_tetrahedral_mesh.m_indices[i + 1]] ,
				new_tetrahedral_mesh.m_vertex_positions[new_tetrahedral_mesh.m_indices[i + 2]] });
		}
		ptr->updateMesh();
	}
}
