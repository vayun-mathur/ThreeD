#include "StructuredBuffer.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <exception>

StructuredBuffer::StructuredBuffer(void* buffer, UINT size_unit, UINT count, RenderSystem* system)
	: m_system(system)
{
	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DYNAMIC;
	buff_desc.ByteWidth = size_unit * count;
	buff_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buff_desc.StructureByteStride = size_unit;
	buff_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = buffer;

	if (FAILED(m_system->m_d3d_device->CreateBuffer(&buff_desc, &init_data, &m_buffer)))
	{
		throw std::exception("StructuredBuffer not created successfully");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = count;


	if (FAILED(system->m_d3d_device->CreateShaderResourceView(m_buffer, &srvDesc, &m_srv)))
	{
		throw std::exception("StructuredBuffer not created successfully");
	}
}

void StructuredBuffer::update(DeviceContextPtr context, void* buffer)
{
	context->m_device_context->UpdateSubresource(this->m_buffer, NULL, NULL, buffer, NULL, NULL);
}

StructuredBuffer::~StructuredBuffer()
{
	if (m_buffer) m_buffer->Release();
}
