#include "PhysicsSystem.h"
#include "PhysicalObject.h"

PhysicsSystem::PhysicsSystem()
{
}

void PhysicsSystem::update(std::vector<PhysicalObjectPtr>& objects, double dt)
{
	this->physicals = objects;

	//TODO: collisions below
	physicals[0]->update({
		Force{vec3(0, -2, 0), vec3(physicals[0]->getLinearPosition().x + 1, physicals[0]->getLinearPosition().y, physicals[0]->getLinearPosition().z)},
		Force{vec3(0, 2, 0), vec3(physicals[0]->getLinearPosition().x - 1, physicals[0]->getLinearPosition().y, physicals[0]->getLinearPosition().z)},
		Force{vec3(0, -9.81, 0), physicals[0]->getLinearPosition()}
		}, dt);
}
