#include "ComputeShader.h"
#include "RenderSystem.h"
#include <exception>

ComputeShader::ComputeShader(void* shader_byte_code, size_t byte_code_size, RenderSystem* system)
    : m_system(system)
{
    HRESULT hres = m_system->m_d3d_device->CreateComputeShader(shader_byte_code, byte_code_size, nullptr, &m_cs);
    if (FAILED(hres)) {
        throw std::exception("VertexShader not created successfully");
    }
}

ComputeShader::~ComputeShader()
{
    m_cs->Release();
}
