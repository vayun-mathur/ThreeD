#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class SwapChain
{
public:
	SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system);

	bool present(bool vsync);
	~SwapChain();
private:
	IDXGISwapChain* m_swap_chain = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class DeviceContext;
};

