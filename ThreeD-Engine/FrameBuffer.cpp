#include "FrameBuffer.h"
#include "RenderSystem.h"
#include "Texture.h"

FrameBuffer::FrameBuffer(UINT width, UINT height, RenderSystem* system)
	: m_system(system)
{
	reloadBuffers(width, height);
}

void FrameBuffer::resize(unsigned int width, unsigned int height)
{
	if (m_rtv) m_rtv->Release();
	if (m_dsv) m_dsv->Release();

	reloadBuffers(width, height);
}

void FrameBuffer::reloadBuffers(unsigned int width, unsigned int height)
{
	ID3D11Device* device = m_system->m_d3d_device;

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	ID3D11Texture2D* texture;

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(result))
	{
		return;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &m_rtv);
	if (FAILED(result))
	{
		return;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return;
	}

	m_texture = std::make_shared<Texture>(texture, m_shaderResourceView);

	D3D11_TEXTURE2D_DESC shadowTexDesc;
	shadowTexDesc.Width = width;
	shadowTexDesc.Height = height;
	shadowTexDesc.MipLevels = 1;
	shadowTexDesc.ArraySize = 1;
	shadowTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowTexDesc.SampleDesc.Count = 1;
	shadowTexDesc.SampleDesc.Quality = 0;
	shadowTexDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowTexDesc.CPUAccessFlags = 0;
	shadowTexDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDsv;
	shadowDsv.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDsv.Texture2D.MipSlice = 0;
	shadowDsv.Flags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSrv;
	shadowSrv.Format = DXGI_FORMAT_R32_FLOAT;
	shadowSrv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadowSrv.Texture2D.MipLevels = shadowTexDesc.MipLevels;
	shadowSrv.Texture2D.MostDetailedMip = 0;

	ID3D11Texture2D* _shadowMapTexture;
	ID3D11DepthStencilView* _shadowMapStencil;
	ID3D11ShaderResourceView* _shadowMapRV;
	result = device->CreateTexture2D(&shadowTexDesc, nullptr, &_shadowMapTexture);
	result = device->CreateDepthStencilView(_shadowMapTexture, &shadowDsv, &_shadowMapStencil);
	result = device->CreateShaderResourceView(_shadowMapTexture, &shadowSrv, &_shadowMapRV);

	m_dsv = _shadowMapStencil;
	if (FAILED(result))
	{
		return;
	}
}

FrameBuffer::~FrameBuffer()
{
	m_rtv->Release();
	m_dsv->Release();
	m_shaderResourceView->Release();
}