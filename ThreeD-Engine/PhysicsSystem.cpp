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
	float ks = 10, kd = 0.2;
	float rest_length = 0.1;
	float dt = 0.001;
	uint32_t size = 8;
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

	body = new SoftBody(vec3(0, 20, 0));
}
/*

vec3* ray_triangle_intersection(
	vec3 R1, vec3 R2, Triangle triangle) {
	vec3 RDir = (R2 - R1).normal();
	vec3 E1 = triangle.p2 - triangle.p1;
	vec3 E2 = triangle.p3 - triangle.p1;
	vec3 N = vec3::cross(E1, E2);
	float det = -vec3::dot(RDir, N);
	float invdet = 1.0 / det;
	vec3 AO = R1 - triangle.p1;
	vec3 DAO = vec3::cross(AO, RDir);
	float u = vec3::dot(E2, DAO) * invdet;
	float v = -vec3::dot(E1, DAO) * invdet;
	float t = vec3::dot(AO, N) * invdet;

	if (det <= 1e-6 || t < 0 || t > (R2-R1).mag() || u < 0 || v < 0 || (u + v)>1) {
		return nullptr;
	}
	return new vec3(R1 + RDir * t);
}

vec3* triangle_triangle_intersection(Triangle t1, Triangle t2) {
	int cnt = 0;
	vec3* ptr = new vec3[2];
	vec3* cmp = nullptr;
	if (cmp = ray_triangle_intersection(t1.p1, t1.p2, t2)) ptr[cnt++] = *cmp;
	if (cmp = ray_triangle_intersection(t1.p2, t1.p3, t2)) ptr[cnt++] = *cmp;
	if (cnt == 2) return ptr;
	if (cmp = ray_triangle_intersection(t1.p1, t1.p3, t2)) ptr[cnt++] = *cmp;
	if (cnt == 2) return ptr;
	if (cmp = ray_triangle_intersection(t2.p1, t2.p2, t1)) ptr[cnt++] = *cmp;
	if (cnt == 2) return ptr;
	if (cmp = ray_triangle_intersection(t2.p2, t2.p3, t1)) ptr[cnt++] = *cmp;
	if (cnt == 2) return ptr;
	if (cmp = ray_triangle_intersection(t2.p1, t2.p3, t1)) ptr[cnt++] = *cmp;
	if (cnt == 2) return ptr;
	return nullptr;
}

void mesh_mesh_intersection_force(PhysicalObjectPtr obj1, PhysicalObjectPtr obj2, float dt) {
	for (Triangle& t1 : obj1->getMesh()->getTriangles()) {
		for (Triangle& t2 : obj2->getMesh()->getTriangles()) {
			vec3* ptr;
			if (ptr = triangle_triangle_intersection(t1, t2)) {
				vec3 n1 = vec3::cross(t1.p3 - t1.p1, t1.p2 - t1.p1);
				vec3 n2 = vec3::cross(t2.p3 - t2.p1, t2.p2 - t2.p1);
				vec3 ptr_avg = (ptr[1] + ptr[0]) / 2;
				obj1->update({ Force{n2-n1, ptr_avg} }, dt);
				obj2->update({ Force{n1 - n2, ptr_avg} }, dt);
			}
		}
	}
}
*/


void PhysicsSystem::update(std::vector<PhysicalObjectPtr>& objects, double dt)
{
	this->physicals = objects;

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setComputeShader(soft_body);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBufferCS(cbuf, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRWStructuredBufferCS(body->getNodeBuffer(), 0);
	
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->compute(1, 1, 1);

	body->update(dt);
	updateMesh(physicals[0]->getMesh(), body);
}