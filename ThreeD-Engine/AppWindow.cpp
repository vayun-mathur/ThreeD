#include "AppWindow.h"
#include <Windows.h>
#include "Vector.h"
#include "InputSystem.h"

struct vertex
{
	vec3 position;
	vec2 texcoord;
};

__declspec(align(16))
struct constant
{
	mat4 m_transform;
	mat4 m_view;
	mat4 m_projection;
	vec4 m_light_direction;
	vec4 m_camera_position;
};

AppWindow::AppWindow()
{
}

void AppWindow::update()
{
	constant cc;

	//light


	mat4 temp;
	mat4 m_light_rot_matrix;
	m_light_rot_matrix.setIdentity();
	m_light_rot_matrix.setRotationY(m_light_rot_y);

	m_light_rot_y += 0.707f * m_delta_time;


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

	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);


	cc.m_projection.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);


	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	m_root = std::make_shared<SceneObject>("root");
	MeshObjectPtr mesh = std::make_shared<MeshObject>("root", L"Assets\\Meshes\\statue.obj", L"Assets\\Textures\\brick.png");
	m_root->addChild(std::reinterpret_pointer_cast<SceneObject>(mesh));

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_world_cam.setTranslation(vec3(0, 0, -1));

	constant cc;

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	m_root->update();

	update();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	m_root->render(m_cb);
	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
}

void AppWindow::onKeyDown(int key)
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

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int width = this->getClientWindowRect().right - this->getClientWindowRect().left;
	int height = this->getClientWindowRect().bottom - this->getClientWindowRect().top;

	m_rot_x += (mouse_pos.y - (height / 2.0f)) * m_delta_time*0.3f;
	m_rot_y += (mouse_pos.x - (width / 2.0f)) * m_delta_time * 0.3f;

	InputSystem::get()->setCursorPosition(Point(width/2, height/2));
}
