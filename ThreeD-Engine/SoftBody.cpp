#include "SoftBody.h"
#include "EditableMesh.h"
#include "GraphicsEngine.h"
#include "RWStructuredBuffer.h"
#define CONVHULL_3D_ENABLE
#include <convhull_3d.h>

SoftBody::SoftBody(vec3 position, int body_size)
{
	std::vector<ch_vertex> vertices;
	std::vector<int> vertex_indices;

	double density = 1;

	double total_size = 2;

	this->size = body_size;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				node n = node{ (float)(density * pow(total_size / size, 3)), vec3(i, j, k) / size * total_size + position};
				n.exists = true;// (vec3(i, j, k) - vec3(0, size - 1, 0)).mag() < size - 1;
				nodes.push_back(n);
				if (n.exists) {
					vertices.push_back(ch_vertex{ (double)i, (double)j, (double)k });
					vertex_indices.push_back(i * size * size + j * size + k);
				}
			}
		}
	}

	int* faceIndices = NULL;
	int nFaces;
	convhull_3d_build(vertices.data(), vertices.size(), &faceIndices, &nFaces);

	for (int i = 0; i < nFaces; i++) {
		triangles.push_back(surface{ vertex_indices[faceIndices[i * 3]], vertex_indices[faceIndices[i * 3 + 1]] , vertex_indices[faceIndices[i * 3 + 2]] });
	}

	nodebuf = GraphicsEngine::get()->getRenderSystem()->createRWStructuredBuffer(&nodes[0], sizeof(node), nodes.size());

	node* ptr = (node*)nodebuf->open_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	memcpy(ptr, &nodes[0], sizeof(node) * nodes.size());
	nodebuf->close_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	nodebuf->toGPU(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());

	auto getNode = [this](float i, float j, float k)->int{return i * size * size + j * size + k; };
	/*
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
	*/
}

void SoftBody::update(float dt)
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				
			}
		}
	}
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