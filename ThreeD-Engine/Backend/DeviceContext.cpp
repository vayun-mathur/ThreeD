#include "DeviceContext.h"
#include "SwapChain.h"

DeviceContext::DeviceContext(ID3D11DeviceContext* device_context) : m_device_context(device_context)
{

}

DeviceContext::~DeviceContext()
{
}

bool DeviceContext::clearRenderTargetColor(SwapChain* swap_chain, float r, float g, float b, float a)
{
	FLOAT color[] = { r, g, b, a };
	m_device_context->ClearRenderTargetView(swap_chain->m_rtv, color);
	return true;
}

bool DeviceContext::release()
{
	m_device_context->Release();
	delete this;
	return true;
}
