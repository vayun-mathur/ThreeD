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
	CameraObject(std::string name, SceneSystem* system, float clip_dist, float speed);

	~CameraObject();

	virtual SceneObjectType getType() const {
		return SceneObjectType::CameraObject;
	}

	inline mat4 getViewMatrix() { return m_view; }
	inline mat4 getProjectionMatrix() { return m_projection; }
	inline vec4 getCameraPosition() { return m_camera_position; }

	void updateProjectionMatrix();
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
	float m_clip_dist;
	float m_speed;

	float m_rot_x = 0;
	float m_rot_y = 0;

	float m_forward = 0.0f;
	float m_rightward = 0.0f;

	float m_delta_time;

	mat4 m_world_cam;

	mat4 m_view;
	mat4 m_projection;
	vec4 m_camera_position;

	bool m_in_play = true;
};

