#pragma once
#include "Vector.h"

class VolumeObject {
public:
	VolumeObject(vec3 position, vec3 scale)
		: position(position), scale(scale) {}

	vec3 getPosition() { return position; }
	void setPosition(vec3 position) { this->position = position; }
	vec3 getScale() { return scale; }
	void setScale(vec3 scale) { this->scale = scale; }
private:
	vec3 position;
	vec3 scale;
};