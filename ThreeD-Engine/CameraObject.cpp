#include "CameraObject.h"
#include "InputSystem.h"
#include "GraphicsEngine.h"
#include "AppWindow.h"

__declspec(align(16))
struct constant
{
	mat4 m_transform;
	mat4 m_view;
	mat4 m_projection;
	vec4 m_light_direction;
	vec4 m_camera_position;
};

CameraObject::CameraObject(std::string name)
	: SceneObject(name)
{
	m_world_cam.setTranslation(vec3(0, 0, -1));

	constant cc;
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

CameraObject::~CameraObject()
{
}

void CameraObject::update(double delta_time)
{
	constant cc;

	//light


	mat4 temp;
	mat4 m_light_rot_matrix;
	m_light_rot_matrix.setIdentity();
	m_light_rot_matrix.setRotationY(m_light_rot_y);

	m_light_rot_y += 0.707f * delta_time;


	cc.m_light_direction = m_light_rot_matrix.getZDirection();

	//transform of object

	cc.m_transform.setIdentity();
	cc.m_transform.setScale(vec3(4, 4, 4));

	//view matrix

	mat4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;


	vec3 new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.02f);

	new_pos = new_pos + world_cam.getXDirection() * (m_rightward * 0.02f);

	world_cam.setTranslation(new_pos);

	cc.m_camera_position = new_pos;

	m_world_cam = world_cam;


	world_cam.inverse();

	cc.m_view = world_cam;

	//projection matrix

	int width = (AppWindow::s_main->getClientWindowRect().right - AppWindow::s_main->getClientWindowRect().left);
	int height = (AppWindow::s_main->getClientWindowRect().bottom - AppWindow::s_main->getClientWindowRect().top);


	cc.m_projection.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);


	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

	m_delta_time = delta_time;
}

void CameraObject::onLeftMouseDown(const Point& mouse_pos)
{
}

void CameraObject::onLeftMouseUp(const Point& mouse_pos)
{
}

void CameraObject::onRightMouseDown(const Point& mouse_pos)
{
}

void CameraObject::onRightMouseUp(const Point& mouse_pos)
{
}

void CameraObject::onKeyDown(int key)
{
	if (key == 'W') {
		m_forward = 1.0f;
	}
	else if (key == 'S') {
		m_forward = -1.0f;
	}
	if (key == 'A') {
		m_rightward = -1.0f;
	}
	else if (key == 'D') {
		m_rightward = 1.0f;
	}
}

void CameraObject::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
}

void CameraObject::onMouseMove(const Point& mouse_pos)
{
	int width = AppWindow::s_main->getClientWindowRect().right - AppWindow::s_main->getClientWindowRect().left;
	int height = AppWindow::s_main->getClientWindowRect().bottom - AppWindow::s_main->getClientWindowRect().top;

	m_rot_x += (mouse_pos.y - (height / 2.0f)) * m_delta_time * 0.3f;
	m_rot_y += (mouse_pos.x - (width / 2.0f)) * m_delta_time * 0.3f;

	InputSystem::get()->setCursorPosition(Point(width / 2, height / 2));
}
