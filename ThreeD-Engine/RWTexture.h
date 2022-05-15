#pragma once
#include "Resource.h"
#include <d3d11.h>

class RWTexture
{
public:
	RWTexture(int size);
	~RWTexture();
private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
private:
	friend class DeviceContext;
};

