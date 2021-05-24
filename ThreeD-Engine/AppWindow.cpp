#include "AppWindow.h"
#include <Windows.h>
#include "Vector.h"
#include "CameraObject.h"
#include "InputSystem.h"

AppWindow* AppWindow::s_main;

AppWindow::AppWindow()
{
	s_main = this;
}

void AppWindow::update()
{
	
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

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	m_root->update(m_delta_time);

	update();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	CameraObjectPtr cam = std::reinterpret_pointer_cast<CameraObject>(m_root->getChild("camera"));
	m_root->render(cam->getConstantBuffer());
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
	InputSystem::get()->addListener(std::reinterpret_pointer_cast<CameraObject>(m_root->getChild("camera")).get());
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
	InputSystem::get()->removeListener(std::reinterpret_pointer_cast<CameraObject>(m_root->getChild("camera")).get());
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
