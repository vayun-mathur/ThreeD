#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class ConstantBuffer
{
public:
	ConstantBuffer(void* buffer, UINT size_buffer, RenderSystem* system);
	void update(DeviceContextPtr context, void* buffer);
	~ConstantBuffer();
private:
	ID3D11Buffer* m_buffer;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class DeviceContext;
};