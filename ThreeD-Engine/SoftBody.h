#pragma once
#include "prerequisites.h"
#include "Vector.h"
#include <vector>

struct node {
	float mass;
	vec3 pos;
	vec3 vel;
	vec3 f;
};

struct surface {
	int p1, p2, p3;
};

class SoftBody {
public:
	SoftBody(vec3 position);

	void update(float dt);
	void editMesh(EditableMeshPtr mesh);
	RWStructuredBufferPtr getNodeBuffer();
private:
	vec3 force(int index_from, int index_by, float rest_length, float ks, float kd);
private:
	std::vector<node> nodes;
	std::vector<surface> triangles;

	RWStructuredBufferPtr nodebuf;
};