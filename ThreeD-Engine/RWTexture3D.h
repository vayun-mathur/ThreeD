#pragma once
#include "Resource.h"
#include <d3d11.h>

class RWTexture3D
{
public:
	RWTexture3D(int size);
	~RWTexture3D();
private:
	ID3D11Texture3D* m_texture = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
private:
	friend class DeviceContext;
};

