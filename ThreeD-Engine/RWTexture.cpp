#include "RWTexture.h"
#include <DirectXTex.h>
#include "GraphicsEngine.h"
#include "DeviceContext.h"

RWTexture::RWTexture(int size)
{

	D3D11_TEXTURE2D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = size;
	descTex.Width = size;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	descTex.CPUAccessFlags = 0;
	// Create texture
	HRESULT hr = (GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture2D(&descTex, 0, &m_texture));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.WSize = size;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

	hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateUnorderedAccessView(m_texture, &uavDesc, &m_uav);
	hr = (GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture, NULL, &m_srv));
}

RWTexture::~RWTexture()
{
	m_uav->Release();
	m_srv->Release();
	m_texture->Release();
}
