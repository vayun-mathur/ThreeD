#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class PixelShader
{
public:
	PixelShader(void* shader_byte_code, size_t byte_code_size, RenderSystem* system);
	~PixelShader();
private:
	ID3D11PixelShader* m_ps;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class RenderSystem;
	friend class DeviceContext;
};

