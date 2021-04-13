#pragma once
#include "Point.h"

class InputListener
{
public:
	InputListener()
	{

	}
	~InputListener()
	{

	}

	//KEYBOARD callback functions
	virtual void onKeyDown(int key) = 0;
	virtual void onKeyUp(int key) = 0;

	//MOUSE callback functions
	virtual void onMouseMove(const Point& mouse_pos) = 0;

	virtual void onLeftMouseDown(const Point& mouse_pos) = 0;
	virtual void onLeftMouseUp(const Point& mouse_pos) = 0;

	virtual void onRightMouseDown(const Point& mouse_pos) = 0;
	virtual void onRightMouseUp(const Point& mouse_pos) = 0;
};