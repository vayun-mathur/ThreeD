#pragma once
#include "Frame.h"
#include <string>

class GUIRect : public Frame {
public:
	GUIRect(std::string name, const Constraints& c, vec4 color, float radius)
		: Frame(name, c), color(color), radius(radius) {

	}
	
	virtual FrameType getType() {
		return FrameType::GUIRect;
	}

	vec4 color;
	float radius;
};