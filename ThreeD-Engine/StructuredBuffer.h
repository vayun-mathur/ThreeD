#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class StructuredBuffer
{
public:
	StructuredBuffer(void* buffer, UINT size_unit, UINT count, RenderSystem* system);
	void update(DeviceContextPtr context, void* buffer);
	~StructuredBuffer();
private:
	ID3D11Buffer* m_buffer;
	ID3D11ShaderResourceView* m_srv;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class DeviceContext;
};