#include "VertexShader.h"
#include "GraphicsEngine.h"

VertexShader::VertexShader()
{
}

bool VertexShader::release()
{
    m_vs->Release();
    delete this;
    return true;
}

VertexShader::~VertexShader()
{
}

bool VertexShader::init(void* shader_byte_code, size_t byte_code_size)
{
    if (FAILED(GraphicsEngine::get()->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &m_vs))) {
        return false;
    }
    return true;
}
