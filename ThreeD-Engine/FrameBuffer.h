#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class FrameBuffer
{
public:
	FrameBuffer(UINT width, UINT height, RenderSystem* system);
	~FrameBuffer();
	void resize(unsigned int width, unsigned int height);

	TexturePtr getTexture() { return m_texture; }
	TexturePtr getDepthTexture() { return m_depth_texture; }
private:
	void reloadBuffers(unsigned int width, unsigned int height);
private:
	TexturePtr m_texture = nullptr;
	TexturePtr m_depth_texture = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class DeviceContext;
};

