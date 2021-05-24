#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include "InputListener.h"
#include "Vector.h"
#include "ConstantBuffer.h"
class CameraObject
	: public SceneObject, public InputListener
{
public:
	CameraObject(std::string name);

	~CameraObject();

	inline ConstantBufferPtr getConstantBuffer() { return m_cb; }

private:
	virtual void update(double delta_time);

private:
	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;

	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;


	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	virtual void onMouseMove(const Point& delta_mouse_pos);

private:
	ConstantBufferPtr m_cb;
private:
	float m_rot_x = 0;
	float m_rot_y = 0;

	float m_light_rot_y = 0.0f;

	float m_forward = 0.0f;
	float m_rightward = 0.0f;

	float m_delta_time;

	mat4 m_world_cam;
};

