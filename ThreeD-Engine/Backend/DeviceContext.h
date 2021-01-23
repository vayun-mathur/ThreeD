#pragma once
#include <d3d11.h>

class SwapChain;
class VertexBuffer;

class DeviceContext
{
public:
	DeviceContext(ID3D11DeviceContext* device_context);
	~DeviceContext();

	void clearRenderTargetColor(SwapChain* swap_chain, float r, float g, float b, float a);
	void setVertexBuffer(VertexBuffer* vertex_buffer);
	void drawTriangleList(UINT vertex_count, UINT start_vertex_index);
	void setViewportSize(UINT width, UINT height);

	bool release();
private:
	ID3D11DeviceContext* m_device_context;
};

