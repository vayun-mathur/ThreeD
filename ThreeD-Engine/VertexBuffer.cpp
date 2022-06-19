#include "VertexBuffer.h"
#include "RenderSystem.h"
#include <exception>

D3D11_INPUT_ELEMENT_DESC layout_mesh[] =
{
	//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
	{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
	{"TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA ,0},
	{"NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,D3D11_INPUT_PER_VERTEX_DATA ,0 },
};

D3D11_INPUT_ELEMENT_DESC layout_terrain[] =
{
	//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
	{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
	{"TEXCOORD", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA ,0},
	{"NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA ,0 },
};

D3D11_INPUT_ELEMENT_DESC layout_water[] =
{
	//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
	{"POSITION", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
};

D3D11_INPUT_ELEMENT_DESC layout_volumetric[] =
{
	//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
	{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
};

D3D11_INPUT_ELEMENT_DESC layout_gui[] =
{
	//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
	{"POSITION", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
};

VertexBuffer::VertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader, RenderSystem* system, int type)
	: m_layout(0), m_buffer(0), m_system(system)
{
	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DYNAMIC;
	buff_desc.ByteWidth = size_vertex * size_list;
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_vertices;

	m_size_vertex = size_vertex;
	m_size_list = size_list;

	if (FAILED(m_system->m_d3d_device->CreateBuffer(&buff_desc, &init_data, &m_buffer)))
	{
		throw std::exception("VertexBuffer not created successfully");
	}

	D3D11_INPUT_ELEMENT_DESC* layout;
	UINT size_layout;

	if (type == 4) layout = layout_gui, size_layout = 1;
	if (type == 3) layout = layout_volumetric, size_layout = 1;
	else if (type == 2) layout = layout_water, size_layout = 1;
	else if (type == 1) layout = layout_terrain, size_layout = 3;
	else layout = layout_mesh, size_layout = 3;

	HRESULT hres;

	if (FAILED(hres = m_system->m_d3d_device->CreateInputLayout(layout, size_layout, shader_byte_code, size_byte_shader, &m_layout)))
	{
		throw std::exception("VertexBuffer not created successfully");
	}
}

UINT VertexBuffer::getSizeVertexList()
{
	return this->m_size_list;
}

void VertexBuffer::editBuffer(void* list_vertices, UINT size_vertex, UINT size_list)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	ID3D11DeviceContext* context;
	m_system->m_d3d_device->GetImmediateContext(&context);
	context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, list_vertices, size_vertex* size_list);
	context->Unmap(m_buffer, 0);
}

VertexBuffer::~VertexBuffer()
{
	m_layout->Release();
	m_buffer->Release();
}
