#pragma once
#include "Resource.h"
#include <d3d11.h>

class Texture3D : public Resource
{
public:
	Texture3D(const wchar_t* full_path, int size);
	Texture3D(ID3D11Texture3D* texture, ID3D11ShaderResourceView* shader_res_view);
	~Texture3D();
private:
	ID3D11Texture3D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_shader_res_view = nullptr;
private:
	friend class DeviceContext;
};

