#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Mesh.h"
#include "TerrainObject.h"
#include "WaterTileObject.h"
#include "Vector.h"
#include "FrameBuffer.h"
#include "SceneSystem.h"
#include "WaterRenderManager.h"
#include "TerrainRenderManager.h"
#include "MeshRenderManager.h"
#include "VolumetricRenderManager.h"

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
	vec4 fog_color;
};

class AppWindow : public Window, public InputListener
{
public:
	static AppWindow* s_main;
public:
	AppWindow();

	void render();
	void renderScene(ConstantBufferPtr cc, FrameBufferPtr toRender);
	void update();

	~AppWindow();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;
	virtual void onSize() override;

	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;

	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;


	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	virtual void onMouseMove(const Point& delta_mouse_pos);
public:
	SwapChainPtr m_swap_chain;
	ConstantBufferPtr m_cb;

	WaterRenderManager* water_manager;
	TerrainRenderManager* terrain_manager;
	MeshRenderManager* mesh_manager;
	VolumetricRenderManager* volumetric_manager;

	SceneSystem* m_scene = nullptr;

	FrameBufferPtr everything;
private:
	float m_old_delta=0;
	float m_new_delta=0;
	float m_delta_time=0;

	float m_light_rot_y = 0.0f;

	bool m_fullscreen = false;
};
