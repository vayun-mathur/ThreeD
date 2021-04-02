#pragma once
#include "Engine/Window/Window.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Engine/GraphicsEngine/SwapChain.h"
#include "Engine/GraphicsEngine/DeviceContext.h"
#include "Engine/GraphicsEngine/VertexBuffer.h"
#include "Engine/GraphicsEngine/IndexBuffer.h"
#include "Engine/GraphicsEngine/ConstantBuffer.h"
#include "Engine/GraphicsEngine/VertexShader.h"
#include "Engine/GraphicsEngine/PixelShader.h"
#include "Engine/InputSystem/InputListener.h"
#include "Engine/Math/Vector.h"

class AppWindow : public Window, public InputListener
{
public:
	AppWindow();

	void update();

	~AppWindow();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;

	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;

	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;


	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	virtual void onMouseMove(const Point& delta_mouse_pos);
private:
	SwapChainPtr m_swap_chain;
	VertexBufferPtr m_vb;
	IndexBufferPtr m_ib;
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	ConstantBufferPtr m_cb;
	TexturePtr m_wood_tex;
private:
	float m_old_delta=0;
	float m_new_delta=0;
	float m_delta_time=0;

	float m_delta_pos=0;
	float m_delta_scale=1;

	float m_rot_x=0;
	float m_rot_y=0;

	float m_scale_cube = 1;

	float m_forward = 0.0f;
	float m_rightward = 0.0f;

	mat4 m_world_cam;
};

