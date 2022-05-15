#pragma once
#include "Vector.h"
#undef max
#undef min

class VolumeObject {
public:
	VolumeObject(vec3 min, vec3 max)
		: min(min), max(max) {}

	vec3 getMin() { return min; }
	void setMin(vec3 min) { this->min = min; }
	vec3 getMax() { return max; }
	void setMax(vec3 max) { this->max = max; }
private:
	vec3 min;
	vec3 max;
};