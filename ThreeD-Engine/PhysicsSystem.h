#pragma once
#include "prerequisites.h"
#include "SceneSystem.h"
#include <vector>

class PhysicsSystem
{
public:
	PhysicsSystem();
	void update(std::vector<PhysicalObjectPtr>& objects, double dt);
private:
	std::vector<PhysicalObjectPtr> physicals;

	ComputeShaderPtr soft_body;
	ConstantBufferPtr cbuf;
};

