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
#include "Vector.h"
#include "SceneSystem.h"

class AppWindow : public Window, public InputListener
{
public:
	static AppWindow* s_main;
public:
	AppWindow();

	void render();
	void update();
	void setConstantBuffer(MeshObject& mesh);

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
private:
	SwapChainPtr m_swap_chain;
	ConstantBufferPtr m_cb;
private:
	SceneSystem* m_scene;
private:
	float m_old_delta=0;
	float m_new_delta=0;
	float m_delta_time=0;

	float m_light_rot_y = 0.0f;

	bool m_fullscreen = false;
};
