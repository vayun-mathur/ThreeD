#include "SoftBody.h"
#include "EditableMesh.h"
#include "GraphicsEngine.h"
#include "RWStructuredBuffer.h"

SoftBody::SoftBody()
{
	int size = 8;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				nodes.push_back(node{ 0.001, vec3(i, j, k) * 0.1 });
			}
		}
	}

	//nodebuf = GraphicsEngine::get()->getRenderSystem()->createRWStructuredBuffer(&nodes[0], sizeof(node), nodes.size());

	auto getNode = [this, size](float i, float j, float k)->node* {return &this->nodes[i * size * size + j * size + k]; };

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

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				bool i1 = i != size - 1;
				bool j1 = j != size - 1;
				bool k1 = k != size - 1;

				float Ks = 10;
				float Kd = 0.2;

				if (i1) springs.push_back(
					spring{ getNode(i + 1, j, k), getNode(i, j, k), Ks, Kd, 0.1 }
				);
				if (j1) springs.push_back(
					spring{ getNode(i, j + 1, k), getNode(i, j, k), Ks, Kd, 0.1 }
				);
				if (k1) springs.push_back(
					spring{ getNode(i, j, k + 1), getNode(i, j, k), Ks, Kd, 0.1 }
				);
				Ks /= 1.5;
				Kd /= 1.5;
				if (i1 && j1) springs.push_back(
					spring{ getNode(i + 1, j + 1, k), getNode(i, j, k), Ks, Kd, 0.1f*sqrtf(2)}
				), springs.push_back(
					spring{ getNode(i + 1, j, k), getNode(i, j + 1, k), Ks, Kd, 0.1f * sqrtf(2) }
				);
				if (i1 && k1) springs.push_back(
					spring{ getNode(i + 1, j, k + 1), getNode(i, j, k), Ks, Kd, 0.1f * sqrtf(2) }
				), springs.push_back(
					spring{ getNode(i + 1, j, k), getNode(i, j, k + 1), Ks, Kd, 0.1f * sqrtf(2) }
				);
				if (j1 && k1) springs.push_back(
					spring{ getNode(i, j + 1, k + 1), getNode(i, j, k), Ks, Kd, 0.1f * sqrtf(2) }
				), springs.push_back(
					spring{ getNode(i, j + 1, k), getNode(i, j, k + 1), Ks, Kd, 0.1f * sqrtf(2) }
				);
			}
		}
	}
}

void SoftBody::update(float dt)
{
	/*nodebuf->toCPU(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	node* ptr = (node*)nodebuf->open_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	nodes = std::vector<node>(ptr, ptr+nodes.size());
	nodebuf->close_data(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext());
	*/
	dt = 0.001;
	for (node& n : nodes) {
		n.f = vec3(0, -9.81*0.001, 0);
	}
	for (spring& s : springs) {
		vec3 dir = (s.e2->pos - s.e1->pos);
		vec3 f = (dir - dir.normal() * s.rest_length) * s.ks
			+ dir.normal() * vec3::dot(dir.normal(), s.e2->vel - s.e1->vel) * s.kd;
		s.e1->f += f;
		s.e2->f -= f;
	}
	for (node& n : nodes) {
		if (n.pos.y <= -2) {
			n.vel.y = 0;
			n.f.y = 0;
		}
	}
	for (node& n : nodes) {
		n.vel += n.f * dt / n.mass;
		n.pos += n.vel * dt;
	}
}

void SoftBody::editMesh(EditableMeshPtr mesh)
{
	mesh->getTriangles().clear();
	for (surface& s : triangles) {
		mesh->getTriangles().push_back(Triangle{ s.p1->pos, s.p2->pos, s.p3->pos });
	}
	mesh->updateMesh();
}
/*
RWStructuredBufferPtr SoftBody::getNodeBuffer()
{
	return nodebuf;
}

*/