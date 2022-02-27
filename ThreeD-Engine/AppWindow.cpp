#include "AppWindow.h"
#include <Windows.h>
#include "Vector.h"
#include "CameraObject.h"
#include "DirectionalLightObject.h"
#include "PointLightObject.h"
#include "ScriptObject.h"
#include "MeshObject.h"
#include "TerrainObject.h"
#include "InputSystem.h"
#include "AudioSystem.h"
#include "AudioSoundManager.h"
#include "AudioSourceObject.h"
#include "ScriptSystem.h"
#include <iostream>

struct dlight {
	vec4 light_direction;
};

struct plight {
	vec4 light_position;
	float light_radius = 0;
	vec3 attenuation;
};

__declspec(align(16))
struct constant
{
	mat4 m_transform;
	mat4 m_view;
	mat4 m_projection;
	vec4 m_camera_position;
	vec4 m_clip;
	int m_dlight_count = 0;
	int m_plight_count = 0;
	int v = 0;
	int x = 0;
	dlight dlight[5];
	plight plight[5];
};

AppWindow* AppWindow::s_main;

AppWindow::AppWindow()
{
	s_main = this;
}

constant cc;

void AppWindow::render()
{
	//camera
	CameraObjectPtr cam = m_scene->getCamera();
	cc.m_view = cam->getViewMatrix();
	cc.m_projection = cam->getProjectionMatrix();
	cc.m_camera_position = cam->getCameraPosition();

	float distance = 2 * cc.m_camera_position.y;

	cc.m_camera_position.y -= distance;
	cam->setCameraPosition(cc.m_camera_position.xyz());
	cam->invertPitch();
	cam->updateMatrices();
	cc.m_view = cam->getViewMatrix();
	cc.m_clip = vec4(0, 1, 0, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_reflection, 0, 0, 0, 1);
	m_scene->render(m_cb);

	cc.m_camera_position.y += distance;
	cam->setCameraPosition(cc.m_camera_position.xyz());
	cam->invertPitch();
	cam->updateMatrices();
	cc.m_view = cam->getViewMatrix();
	cc.m_clip = vec4(0, -1, 0, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_refraction, 0, 0, 0, 1);
	m_scene->render(m_cb);

	cc.m_clip = vec4(0, 0, 0, 100000);
	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 0, 0, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	m_scene->render(m_cb);


	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = (float)::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::update()
{
	CameraObjectPtr cam = m_scene->getCamera();
	MeshObjectPtr skybox = m_scene->getRoot()->getChild<MeshObject>("skybox");
	skybox->setPosition(vec3(cam->getCameraPosition().x, cam->getCameraPosition().y, cam->getCameraPosition().z));
	skybox->setScale(vec3(100, 100, 100));
}

void findLights(SceneObjectPtr obj, std::vector<DirectionalLightObjectPtr>& dlights, std::vector<PointLightObjectPtr>& plights) {
	if (obj->getType()==SceneObjectType::DirectionalLightObject) {
		dlights.push_back(std::dynamic_pointer_cast<DirectionalLightObject>(obj));
	}
	if (obj->getType() == SceneObjectType::PointLightObject) {
		plights.push_back(std::dynamic_pointer_cast<PointLightObject>(obj));
	}
	for (auto&&[_, child] : obj->getChildren()) {
		findLights(child, dlights, plights);
	}

}

void AppWindow::setConstantBuffer(MeshObject& mesh)
{

	std::vector<DirectionalLightObjectPtr> dlights;
	std::vector<PointLightObjectPtr> plights;

	findLights(m_scene->getRoot(), dlights, plights);

	for (int i = 0; i < dlights.size(); i++) {
		cc.dlight[i].light_direction = dlights[i]->getDirection();
	}
	cc.m_dlight_count = (int)dlights.size();

	for (int i = 0; i < plights.size(); i++) {
		cc.plight[i].light_position = plights[i]->getPosition();
		cc.plight[i].light_radius = plights[i]->getRadius();
		cc.plight[i].attenuation = plights[i]->getAttenuation();
	}
	cc.m_plight_count = (int)plights.size();

	//transform
	cc.m_transform.setIdentity();
	cc.m_transform.setTranslation(mesh.getPosition());
	cc.m_transform.setScale(mesh.getScale());

	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void AppWindow::setConstantBuffer(TerrainObject& terrain)
{

	std::vector<DirectionalLightObjectPtr> dlights;
	std::vector<PointLightObjectPtr> plights;

	findLights(m_scene->getRoot(), dlights, plights);

	for (int i = 0; i < dlights.size(); i++) {
		cc.dlight[i].light_direction = dlights[i]->getDirection();
	}
	cc.m_dlight_count = (int)dlights.size();

	for (int i = 0; i < plights.size(); i++) {
		cc.plight[i].light_position = plights[i]->getPosition();
		cc.plight[i].light_radius = plights[i]->getRadius();
		cc.plight[i].attenuation = plights[i]->getAttenuation();
	}
	cc.m_plight_count = (int)plights.size();

	//transform
	cc.m_transform.setIdentity();
	cc.m_transform.setTranslation(terrain.getPosition());
	cc.m_transform.setScale(terrain.getScale());

	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void AppWindow::setConstantBuffer(WaterTileObject& water_tile)
{

	std::vector<DirectionalLightObjectPtr> dlights;
	std::vector<PointLightObjectPtr> plights;

	findLights(m_scene->getRoot(), dlights, plights);

	for (int i = 0; i < dlights.size(); i++) {
		cc.dlight[i].light_direction = dlights[i]->getDirection();
	}
	cc.m_dlight_count = (int)dlights.size();

	for (int i = 0; i < plights.size(); i++) {
		cc.plight[i].light_position = plights[i]->getPosition();
		cc.plight[i].light_radius = plights[i]->getRadius();
		cc.plight[i].attenuation = plights[i]->getAttenuation();
	}
	cc.m_plight_count = (int)plights.size();

	//transform
	cc.m_transform.setIdentity();
	cc.m_transform.setTranslation(water_tile.getPosition());
	cc.m_transform.setScale(water_tile.getScale());

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
	m_scene->init();

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	constant cc;
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));

	AudioSoundPtr audio = AudioSystem::get()->getAudioSoundManager()->createAudioSoundFromFile(L"Assets/Audio/CantinaBand60.wav");

	//m_scene->getRoot()->m_click = *m_scene->getRoot()->getChild<ScriptObject>("script")->getScript();
	//m_scene->getRoot()->getChild<AudioSourceObject>("audio")->play(audio);
	// m_scene->getRoot()->getChild<AudioSourceObject>("audio")->setPitch(1.5);
	// m_scene->getRoot()->getChild<AudioSourceObject>("audio")->setGain(0.2);

	m_reflection = std::make_shared<FrameBuffer>(rc.right - rc.left, rc.bottom - rc.top, GraphicsEngine::get()->getRenderSystem());
	m_refraction = std::make_shared<FrameBuffer>(rc.right - rc.left, rc.bottom - rc.top, GraphicsEngine::get()->getRenderSystem());

	onFocus();
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();
	m_scene->update(m_delta_time);
	/*
	auto audio = m_scene->getRoot()->getChild<AudioSourceObject>("audio");
	vec3 v = audio->getPosition();
	v.x += 0.1f;
	audio->setPosition(v);
	*/
	vec3 v = m_scene->getCamera()->getCameraPosition();
	std::cout << v.x << " " << v.y << " " << v.z << std::endl;

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
