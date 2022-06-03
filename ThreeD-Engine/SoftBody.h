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

struct spring {
	node* e1, * e2;
	float ks, kd;
	float rest_length;
};

struct surface {
	node* p1, *p2, *p3;
};

class SoftBody {
public:
	SoftBody();

	void update(float dt);
	void editMesh(EditableMeshPtr mesh);
private:
	std::vector<node> nodes;
	std::vector<spring> springs;
	std::vector<surface> triangles;
};