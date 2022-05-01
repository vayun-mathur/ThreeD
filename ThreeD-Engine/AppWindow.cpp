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
#include "VolumeObject.h"
#include <iostream>

AppWindow* AppWindow::s_main;

AppWindow::AppWindow()
{
	s_main = this;
}

constant cc;

void find(SceneObjectPtr obj, std::vector<MeshObjectPtr>& meshes,
	std::vector<TerrainObjectPtr>& terrains,
	std::vector<WaterTileObjectPtr>& waters,
	std::vector<DirectionalLightObjectPtr>& dlights,
	std::vector<PointLightObjectPtr>& plights, MeshObjectPtr* skybox) {
	if (obj->getType() == SceneObjectType::MeshObject) {
		if (obj->getName() == "skybox") {
			*skybox = std::dynamic_pointer_cast<MeshObject>(obj);
		}
		else {
			meshes.push_back(std::dynamic_pointer_cast<MeshObject>(obj));
		}
	}
	if (obj->getType() == SceneObjectType::TerrainObject) {
		terrains.push_back(std::dynamic_pointer_cast<TerrainObject>(obj));
	}
	if (obj->getType() == SceneObjectType::WaterTileObject) {
		waters.push_back(std::dynamic_pointer_cast<WaterTileObject>(obj));
	}
	if (obj->getType() == SceneObjectType::DirectionalLightObject) {
		dlights.push_back(std::dynamic_pointer_cast<DirectionalLightObject>(obj));
	}
	if (obj->getType() == SceneObjectType::PointLightObject) {
		plights.push_back(std::dynamic_pointer_cast<PointLightObject>(obj));
	}
	for (auto&& [_, child] : obj->getChildren()) {
		find(child, meshes, terrains, waters, dlights, plights, skybox);
	}
}

void AppWindow::renderScene(ConstantBufferPtr cb) {
	std::vector<MeshObjectPtr> meshes;
	std::vector<TerrainObjectPtr> terrains;
	std::vector<WaterTileObjectPtr> waters;
	std::vector<DirectionalLightObjectPtr> dlights;
	std::vector<PointLightObjectPtr> plights;
	std::vector<VolumeObjectPtr> volumes;
	MeshObjectPtr skybox;
	find(m_scene->getRoot(), meshes, terrains, waters, dlights, plights, &skybox);

	//set lights
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
	mesh_manager->render(meshes, m_cb, cc);
	mesh_manager->render_skybox(skybox, m_cb, cc);

	terrain_manager->render(terrains, m_cb, cc);
	
	water_manager->render(waters, m_cb, cc);
	volumes.push_back(std::make_shared<VolumeObject>(vec3(0, 3, 0), vec3(2, 2, 2)));
	volumes.push_back(std::make_shared<VolumeObject>(vec3(5, 3, 0), vec3(2, 1, 2)));
	volumetric_manager->render(volumes);
}

void AppWindow::render()
{
	cc.fog_color = vec4(0.2, 0.2, 0.2, 1);

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
	water_manager->setReflectionTexture();
	renderScene(m_cb);

	cc.m_camera_position.y += distance;
	cam->setCameraPosition(cc.m_camera_position.xyz());
	cam->invertPitch();
	cam->updateMatrices();
	cc.m_view = cam->getViewMatrix();
	cc.m_clip = vec4(0, -1, 0, 0);
	water_manager->setRefractionTexture();
	renderScene(m_cb);

	cc.m_clip = vec4(0, 0, 0, 100000);
	//CLEAR THE RENDER TARGET
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 0, 0, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	renderScene(m_cb);


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
	skybox->setScale(vec3(10000, 10000, 10000));

	water_manager->update(m_delta_time);
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

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));

	water_manager = new WaterRenderManager();
	water_manager->init(rc);
	terrain_manager = new TerrainRenderManager();
	terrain_manager->init();
	mesh_manager = new MeshRenderManager();
	mesh_manager->init();
	volumetric_manager = new VolumetricRenderManager();
	volumetric_manager->init();

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
