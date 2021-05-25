#include "AppWindow.h"
#include <Windows.h>
#include "Vector.h"
#include "CameraObject.h"
#include "DirectionalLightObject.h"
#include "InputSystem.h"

__declspec(align(16))
struct constant
{
	mat4 m_transform;
	mat4 m_view;
	mat4 m_projection;
	vec4 m_light_direction;
	vec4 m_camera_position;
};

AppWindow* AppWindow::s_main;

AppWindow::AppWindow()
{
	s_main = this;
}

void AppWindow::render()
{
	setConstantBuffer();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	m_scene->render(m_cb);
	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::update()
{
	//light
	DirectionalLightObjectPtr light = m_scene->getLight();

	mat4 m_light_rot_matrix;
	m_light_rot_matrix.setIdentity();
	m_light_rot_matrix.setRotationY(m_light_rot_y);

	m_light_rot_y += 0.707f * m_delta_time;

	light->setDirection(m_light_rot_matrix.getZDirection());
}

void AppWindow::setConstantBuffer()
{
	constant cc;

	//light
	DirectionalLightObjectPtr light = m_scene->getLight();
	cc.m_light_direction = light->getDirection();

	//transform
	cc.m_transform.setIdentity();
	cc.m_transform.setScale(vec3(4, 4, 4));

	//camera
	CameraObjectPtr cam = m_scene->getCamera();
	cc.m_view = cam->getViewMatrix();
	cc.m_projection = cam->getProjectionMatrix();
	cc.m_camera_position = cam->getCameraPosition();


	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->showCursor(false);

	m_scene = new SceneSystem(L"scene.txt");

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	constant cc;
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));

	onFocus();
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();
	m_scene->update(m_delta_time);

	update();
	render();
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->setFullscreen(false, 1, 1);
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
	InputSystem::get()->addListener(m_scene->getCamera().get());
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
	InputSystem::get()->removeListener(m_scene->getCamera().get());
}

void AppWindow::onSize()
{
	RECT rc = this->getClientWindowRect();
	m_swap_chain->resize(rc.right, rc.bottom);
	m_scene->getCamera()->updateProjectionMatrix();
	render();
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
}

void AppWindow::onKeyUp(int key)
{
	if (key == 'F') {
		m_fullscreen = !m_fullscreen;
		RECT size_screen = this->getScreenSize();
		m_swap_chain->setFullscreen(m_fullscreen, size_screen.right, size_screen.bottom);
		int width = AppWindow::s_main->getClientWindowRect().right - AppWindow::s_main->getClientWindowRect().left;
		int height = AppWindow::s_main->getClientWindowRect().bottom - AppWindow::s_main->getClientWindowRect().top;
		InputSystem::get()->setCursorPosition(Point(width / 2, height / 2));
	}
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
}
