#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class RWStructuredBuffer
{
public:
	RWStructuredBuffer(UINT size_unit, UINT count, RenderSystem* system, UINT uav_flags = 0);
	void update(DeviceContextPtr context, void* buffer);

	void toCPU(DeviceContextPtr context);
	void toGPU(DeviceContextPtr context);
	D3D11_MAPPED_SUBRESOURCE open_data(DeviceContextPtr context);
	void close_data(DeviceContextPtr context);

	~RWStructuredBuffer();
private:
	ID3D11Buffer* m_buffer;
	ID3D11Buffer* m_bufferCPU;
	ID3D11UnorderedAccessView* m_uav;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class DeviceContext;
};