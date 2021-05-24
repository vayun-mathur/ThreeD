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

void AppWindow::update()
{
	//light
	DirectionalLightObjectPtr light = m_root->getChild<DirectionalLightObject>("light");

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
	DirectionalLightObjectPtr light = m_root->getChild<DirectionalLightObject>("light");
	cc.m_light_direction = light->getDirection();

	//transform
	cc.m_transform.setIdentity();
	cc.m_transform.setScale(vec3(4, 4, 4));

	//camera
	CameraObjectPtr cam = m_root->getChild<CameraObject>("camera");
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

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	m_root = std::make_shared<SceneObject>("root");
	MeshObjectPtr mesh = std::make_shared<MeshObject>("mesh", L"Assets\\Meshes\\statue.obj", L"Assets\\Textures\\brick.png");
	m_root->addChild(mesh);
	CameraObjectPtr camera = std::make_shared<CameraObject>("camera");
	InputSystem::get()->addListener(camera.get());
	m_root->addChild(camera);
	DirectionalLightObjectPtr light = std::make_shared<DirectionalLightObject>("light", vec3(1, 1, 1), vec3(0, 0, 1));
	m_root->addChild(light);

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	constant cc;
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	m_root->update(m_delta_time);

	update();
	setConstantBuffer();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	m_root->render(m_cb);
	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
	InputSystem::get()->addListener(m_root->getChild<CameraObject>("camera").get());
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
	InputSystem::get()->removeListener(m_root->getChild<CameraObject>("camera").get());
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
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
}
