#include "PhysicsSystem.h"
#include "PhysicalObject.h"
#include "EditableMesh.h"
#include "SoftBody.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

SoftBody* body;

__declspec(align(16))
struct cbufC
{
	float ks = 10, kd = 0.5;
	float rest_length = 0.1;
	float dt = 0.0002;
	uint32_t size = 20;
};

cbufC buf;

void updateMesh(EditableMeshPtr mesh, SoftBody* body) {
	body->editMesh(mesh);
}

PhysicsSystem::PhysicsSystem()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileComputeShader(L"SoftBody.hlsl", "UpdateSprings", &shader_byte_code, &size_shader);
	soft_body = GraphicsEngine::get()->getRenderSystem()->createComputeShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	cbuf = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(cbufC));

	body = new SoftBody(vec3(0, 20, 0), buf.size);
}

void PhysicsSystem::update(std::vector<PhysicalObjectPtr>& objects, double dt)
{
	this->physicals = objects;

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setComputeShader(soft_body);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferCS(cbuf, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRWStructuredBufferCS(body->getNodeBuffer(), 0);
	
	for (int i = 0; i < 10; i++) {
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->compute(4, 4, 4);
	}
	
	body->update(dt);
	updateMesh(physicals[0]->getMesh(), body);
}
