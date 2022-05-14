#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class ComputeShader
{
public:
	ComputeShader(void* shader_byte_code, size_t byte_code_size, RenderSystem* system);
	~ComputeShader();
private:
	ID3D11ComputeShader* m_cs;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class RenderSystem;
	friend class DeviceContext;
	friend class AppWindow;
};

