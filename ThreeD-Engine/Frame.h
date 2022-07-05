#pragma once
#include "vector.h"
#include <vector>
#include "prerequisites.h"

struct FrameData {
	vec2 position;
	vec2 scale;
	float z;
};

enum class FrameType {
	Frame, GUIRect
};

class PositionalConstraint {
};

class Constraints {
public:
	float (*left)() = nullptr;
	float (*xcenter)() = nullptr;
	float (*right)() = nullptr;
	float (*width)() = nullptr;

	float (*top)() = nullptr;
	float (*ycenter)() = nullptr;
	float (*bottom)() = nullptr;
	float (*height)() = nullptr;

	bool verify() {
		int x = 0;
		x += left != nullptr;
		x += xcenter != nullptr;
		x += right != nullptr;
		x += width != nullptr;

		int y = 0;
		y += top != nullptr;
		y += ycenter != nullptr;
		y += bottom != nullptr;
		y += height != nullptr;

		return x == 2 && y == 2;
	}

	FrameData getData() {
		FrameData data;
		if (!verify()) return data;
		if (left && xcenter) {
			float l = left();
			float xcen = xcenter();
			data.position.x = xcen;
			data.scale.x = xcen-l;
		}
		if (left && right) {
			float l = left();
			float r = right();
			data.position.x = (r + l)/2;
			data.scale.x = (r - l)/2;
		}
		if (left && width) {
			float l = left();
			float w = width();
			data.position.x = l + w/2;
			data.scale.x = w;
		}
		if (xcenter && right) {
			float xcen = xcenter();
			float r = right();
			data.position.x = xcen;
			data.scale.x = r - xcen;
		}
		if (xcenter && width) {
			float xcen = xcenter();
			float w = width();
			data.position.x = xcen;
			data.scale.x = w;
		}
		if (right && width) {
			float r = right();
			float w = width();
			data.position.x = r - w/2;
			data.scale.x = w;
		}


		if (top && ycenter) {
			float t = top();
			float ycen = ycenter();
			data.position.y = ycen;
			data.scale.y = ycen - t;
		}
		if (top && bottom) {
			float t = top();
			float b = bottom();
			data.position.y = (b + t) / 2;
			data.scale.y = (b - t) / 2;
		}
		if (top && height) {
			float t = top();
			float h = height();
			data.position.y = t + h / 2;
			data.scale.y = h;
		}
		if (ycenter && bottom) {
			float ycen = ycenter();
			float b = bottom();
			data.position.y = ycen;
			data.scale.y = b - ycen;
		}
		if (ycenter && height) {
			float ycen = ycenter();
			float h = height();
			data.position.y = ycen;
			data.scale.y = h;
		}
		if (bottom && height) {
			float b = bottom();
			float h = height();
			data.position.y = b - h / 2;
			data.scale.y = h;
		}

		return data;
	}
	// left constraint, center constraint, right constraint
	// top constraint, center constraint, bottom constraint
	// width constraint
	// height constraint
};

class Frame {
public:
	Frame(std::string name, const Constraints& constraints):
		name(name), constraints(constraints) {

	}
	FrameData getRealCoordinates() {
		FrameData data = constraints.getData();
		if (parent == nullptr) {
			return { data.position, data.scale, 0 };
		}
		else {
			FrameData parent_data = parent->getRealCoordinates();
			return { parent_data.position + data.position * parent_data.scale, data.scale * parent_data.scale, parent_data.z+0.01f };
		}
	}

	virtual FrameType getType() {
		return FrameType::Frame;
	}

	void addChild(FramePtr child) {
		children[child->name] = child;
		child->parent = FramePtr(this);
	}

	FramePtr getChild(std::string name) { return children[name]; }

	const std::map<std::string, FramePtr>& getChildren() { return children; };
private:
	std::string name;
	FramePtr parent;
	std::map<std::string, FramePtr> children;
	Constraints constraints;
};