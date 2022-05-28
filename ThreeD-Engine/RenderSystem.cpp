#include "RenderSystem.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include "FrameBuffer.h"
#include "StructuredBuffer.h"
#include "RWStructuredBuffer.h"

#include <d3dcompiler.h>

#include <iostream>

RenderSystem::RenderSystem()
{
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	HRESULT res = 0;
	for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
	{
		res = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, NULL, feature_levels,
			num_feature_levels, D3D11_SDK_VERSION, &m_d3d_device, &m_feature_level, &m_imm_context);
		if (SUCCEEDED(res))
			break;
		++driver_type_index;
	}
	if (FAILED(res))
	{
		throw std::exception("RenderSystem not created successfully");
	}

	m_imm_device_context = std::make_shared<DeviceContext>(m_imm_context, this);

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

	initRasterizerState();
}

RenderSystem::~RenderSystem()
{
	if (m_vs)m_vs->Release();
	if (m_ps)m_ps->Release();

	if (m_vsblob)m_vs->Release();
	if (m_psblob)m_ps->Release();

	m_dxgi_device->Release();
	m_dxgi_adapter->Release();
	m_dxgi_factory->Release();

	m_d3d_device->Release();
}

SwapChainPtr RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height) {
	SwapChainPtr sc = nullptr;
	try
	{
		sc = std::make_shared<SwapChain>(hwnd, width, height, this);
	} 
	catch (...) {}
	return sc;
}

DeviceContextPtr RenderSystem::getImmediateDeviceContext()
{
	return m_imm_device_context;
}

VertexBufferPtr RenderSystem::createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader, int type)
{
	VertexBufferPtr vb = nullptr;
	try
	{
		vb = std::make_shared<VertexBuffer>(list_vertices, size_vertex, size_list, shader_byte_code, size_byte_shader, this, type);
	}
	catch (...) {}
	return vb;
}

IndexBufferPtr RenderSystem::createIndexBuffer(void* list_indices, UINT size_list)
{
	IndexBufferPtr ib = nullptr;
	try
	{
		ib = std::make_shared<IndexBuffer>(list_indices, size_list, this);
	}
	catch (...) {}
	return ib;
}

ConstantBufferPtr RenderSystem::createConstantBuffer(void* buffer, UINT size_buffer)
{
	ConstantBufferPtr cb = nullptr;
	try
	{
		cb = std::make_shared<ConstantBuffer>(buffer, size_buffer, this);
	}
	catch (...) {}
	return cb;
}

StructuredBufferPtr RenderSystem::createStructuredBuffer(void* buffer, UINT size_unit, UINT count)
{
	StructuredBufferPtr sb = nullptr;
	try
	{
		sb = std::make_shared<StructuredBuffer>(buffer, size_unit, count, this);
	}
	catch (...) {}
	return sb;
}

RWStructuredBufferPtr RenderSystem::createAppendStructuredBuffer(void* buffer, UINT size_unit, UINT count)
{
	RWStructuredBufferPtr sb = nullptr;
	try
	{
		sb = std::make_shared<RWStructuredBuffer>(buffer, size_unit, count, this, D3D11_BUFFER_UAV_FLAG_APPEND);
	}
	catch (...) {}
	return sb;
}

RWStructuredBufferPtr RenderSystem::createRWStructuredBuffer(void* buffer, UINT size_unit, UINT count)
{
	RWStructuredBufferPtr sb = nullptr;
	try
	{
		sb = std::make_shared<RWStructuredBuffer>(buffer, size_unit, count, this);
	}
	catch (...) {}
	return sb;
}

VertexShaderPtr RenderSystem::createVertexShader(void* shader_byte_code, size_t byte_code_size)
{
	VertexShaderPtr vs = nullptr;
	try
	{
		vs = std::make_shared<VertexShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}
	return vs;
}

PixelShaderPtr RenderSystem::createPixelShader(void* shader_byte_code, size_t byte_code_size)
{
	PixelShaderPtr ps = nullptr;
	try
	{
		ps = std::make_shared<PixelShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}
	return ps;
}

ComputeShaderPtr RenderSystem::createComputeShader(void* shader_byte_code, size_t byte_code_size)
{
	ComputeShaderPtr cs = nullptr;
	try
	{
		cs = std::make_shared<ComputeShader>(shader_byte_code, byte_code_size, this);
	}
	catch (...) {}
	return cs;
}

FrameBufferPtr RenderSystem::createFrameBuffer(UINT width, UINT height)
{
	FrameBufferPtr fb = nullptr;
	try
	{
		fb = std::make_shared<FrameBuffer>(width, height, this);
	}
	catch (...) {}
	return fb;
}

bool RenderSystem::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (FAILED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "vs_5_0", 0, 0, &m_blob, &error_blob))) {

		if (error_blob)
		{
			std::cout << (char*)error_blob->GetBufferPointer() << std::endl;
			error_blob->Release();
		}
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();
	return true;
}

bool RenderSystem::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (FAILED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "ps_5_0", 0, 0, &m_blob, &error_blob))) {
		if (error_blob)
		{
			std::cout << (char*)error_blob->GetBufferPointer() << std::endl;
			error_blob->Release();
		}
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();
	return true;
}

bool RenderSystem::compileComputeShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (FAILED(::D3DCompileFromFile(file_name, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point_name, "cs_5_0", 0, 0, &m_blob, &error_blob))) {
		if (error_blob)
		{
			std::cout << (char*)error_blob->GetBufferPointer() << std::endl;
			error_blob->Release();
		}
		return false;
	}
	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();
	return true;
}

void RenderSystem::releaseCompiledShader()
{
	if (m_blob)m_blob->Release();
}

void RenderSystem::setRasterizerState(CULL_MODE cull_mode)
{
	if (cull_mode == CULL_MODE::FRONT)
		m_imm_context->RSSetState(m_cull_front);
	else
		m_imm_context->RSSetState(m_cull_back);
}

void RenderSystem::initRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_FRONT;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;

	m_d3d_device->CreateRasterizerState(&desc, &m_cull_front);

	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;

	m_d3d_device->CreateRasterizerState(&desc, &m_cull_back);
}
