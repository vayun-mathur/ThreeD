#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "Vector.h"
class DirectionalLightObject
	: public SceneObject
{
public:
	DirectionalLightObject(std::string name, vec3 direction);

	~DirectionalLightObject();
	
private:
	vec3 m_direction;
};

