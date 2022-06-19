#pragma once
#include "prerequisites.h"
#include "Vector.h"
#include <vector>

struct node {
	float mass;
	vec3 pos;
	vec3 vel;
	vec3 f;
	int exists;
};

struct surface {
	int p1, p2, p3;
};

class SoftBody {
public:
	SoftBody(vec3 position, int body_size);

	void update(float dt);
	void editMesh(EditableMeshPtr mesh);
	RWStructuredBufferPtr getNodeBuffer();
private:
	std::vector<node> nodes;
	std::vector<surface> triangles;

	RWStructuredBufferPtr nodebuf;

	int size;
};