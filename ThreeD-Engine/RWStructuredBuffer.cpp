#include "RWStructuredBuffer.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <exception>

RWStructuredBuffer::RWStructuredBuffer(UINT size_unit, UINT count, RenderSystem* system, UINT uav_flags)
	: m_system(system)
{
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = size_unit * count;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = size_unit;
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HRESULT hr = (system->m_d3d_device->CreateBuffer(&outputDesc, 0, &m_buffer));

	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = (system->m_d3d_device->CreateBuffer(&outputDesc, 0, &m_bufferCPU));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = uav_flags;
	uavDesc.Buffer.NumElements = count;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	hr = system->m_d3d_device->CreateUnorderedAccessView(m_buffer, &uavDesc, &m_uav);
}

void RWStructuredBuffer::update(DeviceContextPtr context, void* buffer)
{
	context->m_device_context->UpdateSubresource(this->m_buffer, NULL, NULL, buffer, NULL, NULL);
}

void RWStructuredBuffer::toCPU(DeviceContextPtr context)
{
	context->m_device_context->CopyResource(m_bufferCPU, m_buffer);
}

void RWStructuredBuffer::toGPU(DeviceContextPtr context)
{
	context->m_device_context->CopyResource(m_buffer, m_bufferCPU);
}

void* RWStructuredBuffer::open_data(DeviceContextPtr context)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->m_device_context->Map(m_bufferCPU, 0, D3D11_MAP_READ, 0, &mappedResource);
	return mappedResource.pData;
}

void RWStructuredBuffer::close_data(DeviceContextPtr context)
{
	context->m_device_context->Unmap(m_bufferCPU, 0);
}

RWStructuredBuffer::~RWStructuredBuffer()
{
	if (m_buffer) m_buffer->Release();
	if (m_bufferCPU) m_bufferCPU->Release();
	if (m_uav) m_uav->Release();
}
