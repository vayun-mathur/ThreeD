#include "SwapChain.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <exception>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system)
	: m_system(system)
{
	ID3D11Device* device = m_system->m_d3d_device;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hwnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.Windowed = TRUE;

	HRESULT hres = m_system->m_dxgi_factory->CreateSwapChain(device, &desc, &m_swap_chain);

	if (FAILED(hres)) {
		throw std::exception("SwapChain not created successfully");
	}

	reloadBuffers(width, height);
}

void SwapChain::setFullscreen(bool fullscreen, unsigned int width, unsigned int height)
{
	resize(width, height);
	m_swap_chain->SetFullscreenState(fullscreen, nullptr);
}

void SwapChain::resize(unsigned int width, unsigned int height)
{
	if (m_rtv) m_rtv->Release();
	if (m_dsv) m_dsv->Release();

	m_swap_chain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	reloadBuffers(width, height);
}

bool SwapChain::present(bool vsync)
{
	m_swap_chain->Present(vsync, NULL);
	return false;
}

SwapChain::~SwapChain()
{
	m_swap_chain->Release();
}

void SwapChain::reloadBuffers(unsigned int width, unsigned int height)
{
	ID3D11Device* device = m_system->m_d3d_device;

	ID3D11Texture2D* buffer;
	HRESULT hres = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

	if (FAILED(hres)) {
		throw std::exception("SwapChain not created successfully");
	}

	hres = device->CreateRenderTargetView(buffer, NULL, &m_rtv);
	buffer->Release();

	if (FAILED(hres)) {
		throw std::exception("SwapChain not created successfully");
	}

	ID3D11DeviceContext* context = NULL;
	device->GetImmediateContext(&context);

	ID3D11BlendState* d3dBlendState;
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	if (FAILED(device->CreateBlendState(&omDesc, &d3dBlendState)))
		return;
	context->OMSetBlendState(d3dBlendState, 0, 0xffffffff);

	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.MipLevels = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.MiscFlags = 0;
	tex_desc.ArraySize = 1;
	tex_desc.CPUAccessFlags = 0;

	hres = device->CreateTexture2D(&tex_desc, nullptr, &buffer);

	if (FAILED(hres)) {
		throw std::exception("SwapChain not created successfully");
	}

	hres = device->CreateDepthStencilView(buffer, NULL, &m_dsv);
	buffer->Release();

	if (FAILED(hres)) {
		throw std::exception("SwapChain not created successfully");
	}
}
