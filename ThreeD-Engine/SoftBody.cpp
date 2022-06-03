#include "SoftBody.h"
#include "EditableMesh.h"
#include "GraphicsEngine.h"
#include "RWStructuredBuffer.h"

SoftBody::SoftBody(vec3 position)
{
	int size = 8;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				nodes.push_back(node{ 0.001, vec3(i, j, k) * 0.1 + position });
			}
		}
	}

	nodebuf = GraphicsEngine::get()->getRenderSystem()->createRWStructuredBuffer(&nodes[0], sizeof(node), nodes.size());

	node* ptr = (node*)nodebuf->open_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	memcpy(ptr, &nodes[0], sizeof(node) * nodes.size());
	nodebuf->close_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	nodebuf->toGPU(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());

	auto getNode = [this, size](float i, float j, float k)->int{return i * size * size + j * size + k; };

	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - 1; j++) {
			triangles.push_back(surface{ getNode(i, j, 0), getNode(i, j + 1, 0) , getNode(i + 1, j, 0) });
			triangles.push_back(surface{ getNode(i, j + 1, 0), getNode(i + 1, j + 1, 0) , getNode(i + 1, j, 0) });
			triangles.push_back(surface{ getNode(i, j + 1, size - 1), getNode(i, j, size - 1) , getNode(i + 1, j, size - 1) });
			triangles.push_back(surface{ getNode(i + 1, j + 1, size - 1), getNode(i, j + 1, size - 1) , getNode(i + 1, j, size - 1) });
		}
	}
	for (int i = 0; i < size - 1; i++) {
		for (int k = 0; k < size - 1; k++) {
			triangles.push_back(surface{ getNode(i, 0, k + 1), getNode(i, 0, k) , getNode(i + 1, 0, k) });
			triangles.push_back(surface{ getNode(i + 1, 0, k + 1), getNode(i, 0, k + 1) , getNode(i + 1, 0, k) });
			triangles.push_back(surface{ getNode(i, size - 1, k), getNode(i, size - 1, k + 1) , getNode(i + 1, size - 1, k) });
			triangles.push_back(surface{ getNode(i, size - 1, k + 1), getNode(i + 1, size - 1, k + 1) , getNode(i + 1, size - 1, k) });
		}
	}
	for (int j = 0; j < size - 1; j++) {
		for (int k = 0; k < size - 1; k++) {
			triangles.push_back(surface{ getNode(0, j, k), getNode(0, j, k + 1) , getNode(0, j + 1, k) });
			triangles.push_back(surface{ getNode(0, j, k + 1), getNode(0, j + 1, k + 1) , getNode(0, j + 1, k) });
			triangles.push_back(surface{ getNode(size - 1, j, k + 1), getNode(size - 1, j, k) , getNode(size - 1, j + 1, k) });
			triangles.push_back(surface{ getNode(size - 1, j + 1, k + 1), getNode(size - 1, j, k + 1), getNode(size - 1, j + 1, k) });
		}
	}
}


vec3 SoftBody::force(int index_from, int index_by, float ks, float kd, float rest_length) {
	vec3 dir = (nodes[index_by].pos - nodes[index_from].pos);
	vec3 f = (dir - dir.normal() * rest_length) * ks
		+ dir.normal() * vec3::dot(dir.normal(), nodes[index_by].vel - nodes[index_from].vel) * kd;
	return f * -1;
}

void SoftBody::update(float dt)
{
	nodebuf->toCPU(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	node* ptr = (node*)nodebuf->open_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	nodes = std::vector<node>(ptr, ptr+nodes.size());
	nodebuf->close_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
}

void SoftBody::editMesh(EditableMeshPtr mesh)
{
	mesh->getTriangles().clear();
	for (surface& s : triangles) {
		mesh->getTriangles().push_back(Triangle{ nodes[s.p1].pos, nodes[s.p2].pos, nodes[s.p3].pos });
	}
	mesh->updateMesh();
}

RWStructuredBufferPtr SoftBody::getNodeBuffer()
{
	return nodebuf;
}