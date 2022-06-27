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
#include "ComputeShader.h"
#include "StructuredBuffer.h"
#include "RWStructuredBuffer.h"
#include "PhysicalObject.h"
#include <iostream>

AppWindow* AppWindow::s_main;

AppWindow::AppWindow()
{
	s_main = this;
}

constant cc;

void find(SceneObjectPtr obj, std::vector<MeshObjectPtr>& meshes,
	std::vector<PhysicalObjectPtr>& physicals,
	std::vector<TerrainObjectPtr>& terrains,
	std::vector<WaterTileObjectPtr>& waters,
	std::vector<DirectionalLightObjectPtr>& dlights,
	std::vector<PointLightObjectPtr>& plights) {
	if (obj->getType() == SceneObjectType::MeshObject) {
		meshes.push_back(std::dynamic_pointer_cast<MeshObject>(obj));
	}
	if (obj->getType() == SceneObjectType::PhysicalObject) {
		physicals.push_back(std::dynamic_pointer_cast<PhysicalObject>(obj));
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
		find(child, meshes, physicals, terrains, waters, dlights, plights);
	}
}

void AppWindow::renderScene(ConstantBufferPtr cb, FrameBufferPtr toRender) {
	std::vector<MeshObjectPtr> meshes;
	std::vector<PhysicalObjectPtr> physicals;
	std::vector<TerrainObjectPtr> terrains;
	std::vector<WaterTileObjectPtr> waters;
	std::vector<DirectionalLightObjectPtr> dlights;
	std::vector<PointLightObjectPtr> plights;
	std::vector<VolumeObjectPtr> volumes;
	find(m_scene->getRoot(), meshes, physicals, terrains, waters, dlights, plights);

	//set lights
	for (int i = 0; i < dlights.size(); i++) {
		cc.dlight[i].light_direction = vec4(dlights[i]->getDirection(), 1);
	}
	cc.m_dlight_count = (int)dlights.size();

	for (int i = 0; i < plights.size(); i++) {
		cc.plight[i].light_position = vec4(plights[i]->getPosition(), 1);
		cc.plight[i].light_radius = plights[i]->getRadius();
		cc.plight[i].attenuation = plights[i]->getAttenuation();
	}
	cc.m_plight_count = (int)plights.size();
	volumes.push_back(std::make_shared<VolumeObject>(vec3(-20, 20, -20), vec3(20, 60, 20)));

	if (toRender == nullptr) {


		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(everything, 0, 0, 0, 0);

		mesh_manager->render(meshes, m_cb, cc);
		mesh_manager->render(physicals, m_cb, cc);

		terrain_manager->render(terrains, m_cb, cc);

		water_manager->render(waters, m_cb, cc);

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(m_swap_chain, 0, 0, 0, 0);

		sky_manager->render(m_scene->getCamera());
		/*
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(m_swap_chain, 0, 0, 0, 0);

		mesh_manager->render(meshes, m_cb, cc);
		mesh_manager->render(physicals, m_cb, cc);

		terrain_manager->render(terrains, m_cb, cc);

		water_manager->render(waters, m_cb, cc);
		sky_manager->render(m_scene->getCamera());

		volumetric_manager->render(volumes);
		*/
	}
	else {
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(toRender, 0, 0, 0, 0);
		mesh_manager->render(meshes, m_cb, cc);
		mesh_manager->render(physicals, m_cb, cc);

		terrain_manager->render(terrains, m_cb, cc);
	}
}

void AppWindow::render()
{
	cc.fog_color = vec4(135 / 255.f, 206 / 255.f, 235 / 255.f, 1);
	cc.screensize = vec2(getClientWindowRect().right - getClientWindowRect().left, getClientWindowRect().bottom - getClientWindowRect().top);

	//camera
	CameraObjectPtr cam = m_scene->getCamera();
	cc.m_view = cam->getViewMatrix();
	cc.m_projection = cam->getProjectionMatrix();
	cc.m_camera_position = vec4(cam->getCameraPosition(), 1);

	float distance = 2 * cc.m_camera_position.y;

	cc.m_camera_position.y -= distance;
	cam->setCameraPosition(cc.m_camera_position.xyz());
	cam->invertPitch();
	cam->updateMatrices();
	cc.m_view = cam->getViewMatrix();
	cc.m_clip = vec4(0, 1, 0, 0);
	renderScene(m_cb, water_manager->getReflectionTexture());

	cc.m_camera_position.y += distance;
	cam->setCameraPosition(cc.m_camera_position.xyz());
	cam->invertPitch();
	cam->updateMatrices();
	cc.m_view = cam->getViewMatrix();
	cc.m_clip = vec4(0, -1, 0, 0);
	renderScene(m_cb, water_manager->getRefractionTexture());

	cc.m_clip = vec4(0, 0, 0, 100000);
	//CLEAR THE RENDER TARGET
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 1, 0, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	renderScene(m_cb, nullptr);

	std::vector<GUI> guis = { {nullptr, vec2(0.5, 0.5), vec2(0.25, 0.25), 0.5} };
	//gui_manager->render(guis);


	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = (float)::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::update()
{
	water_manager->update(m_delta_time);

	m_scene->update(m_delta_time);

	std::vector<MeshObjectPtr> meshes;
	std::vector<PhysicalObjectPtr> physicals;
	std::vector<TerrainObjectPtr> terrains;
	std::vector<WaterTileObjectPtr> waters;
	std::vector<DirectionalLightObjectPtr> dlights;
	std::vector<PointLightObjectPtr> plights;
	std::vector<VolumeObjectPtr> volumes;
	find(m_scene->getRoot(), meshes, physicals, terrains, waters, dlights, plights);

	physics_system->update(physicals, m_delta_time);
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
	sky_manager = new SkyRenderManager();
	sky_manager->init();
	gui_manager = new GUIRenderManager();
	gui_manager->init();
	physics_system = new PhysicsSystem();

	onFocus();

	everything = GraphicsEngine::get()->getRenderSystem()->createFrameBuffer(rc.right - rc.left, rc.bottom - rc.top);
	sky_pp = GraphicsEngine::get()->getRenderSystem()->createFrameBuffer(rc.right - rc.left, rc.bottom - rc.top);
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

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
	if (m_scene != nullptr)
		InputSystem::get()->addListener(m_scene->getCamera().get());
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
	if(m_scene != nullptr)
		InputSystem::get()->removeListener(m_scene->getCamera().get());
}

void AppWindow::onSize()
{
	RECT rc = this->getClientWindowRect();
	m_swap_chain->resize(rc.right, rc.bottom);
	everything->resize(rc.right, rc.bottom);
	sky_pp->resize(rc.right, rc.bottom);
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
