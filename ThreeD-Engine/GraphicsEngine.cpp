#include "GraphicsEngine.h"
#include "RenderSystem.h"

#include <d3dcompiler.h>


GraphicsEngine* GraphicsEngine::m_engine = nullptr;

GraphicsEngine::GraphicsEngine()
{
	try {
		m_render_system = new RenderSystem();
	}
	catch (...) { throw std::exception("RenderSystem not created successfully"); }
	try {
		m_texture_manager = new TextureManager();
	}
	catch (...) { throw std::exception("TextureManager not created successfully"); }
	try {
		m_mesh_manager = new MeshManager();
	}
	catch (...) { throw std::exception("MeshManager not created successfully"); }

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	m_render_system->compileVertexShader(L"VertexMeshLayoutShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code[0], shader_byte_code, size_shader);
	m_mesh_layout_size[0] = size_shader;
	m_render_system->releaseCompiledShader();

	m_render_system->compileVertexShader(L"VertexMeshLayoutShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code[1], shader_byte_code, size_shader);
	m_mesh_layout_size[1] = size_shader;
	m_render_system->releaseCompiledShader();

	m_render_system->compileVertexShader(L"VertexWaterLayoutShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code[2], shader_byte_code, size_shader);
	m_mesh_layout_size[2] = size_shader;
	m_render_system->releaseCompiledShader();

	m_render_system->compileVertexShader(L"CloudShader.hlsl", "RayCastVS", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code[3], shader_byte_code, size_shader);
	m_mesh_layout_size[3] = size_shader;
	m_render_system->releaseCompiledShader();

	m_render_system->compileVertexShader(L"GUIShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code[4], shader_byte_code, size_shader);
	m_mesh_layout_size[4] = size_shader;
	m_render_system->releaseCompiledShader();
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
}

TextureManager* GraphicsEngine::getTextureManager()
{
	return m_texture_manager;
}

MeshManager* GraphicsEngine::getMeshManager()
{
	return m_mesh_manager;
}

void GraphicsEngine::getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size, int type)
{
	*byte_code = m_mesh_layout_byte_code[type];
	*size = m_mesh_layout_size[type];
}

GraphicsEngine::~GraphicsEngine()
{
	GraphicsEngine::m_engine = nullptr;
	delete m_render_system;
	delete m_texture_manager;
	delete m_mesh_manager;
}

GraphicsEngine* GraphicsEngine::get()
{
	return m_engine;
}

void GraphicsEngine::create()
{
	if (GraphicsEngine::m_engine) throw std::exception("Graphics Engine already created");
	GraphicsEngine::m_engine = new GraphicsEngine();
}

void GraphicsEngine::release()
{
	if (!GraphicsEngine::m_engine) return;
	delete GraphicsEngine::m_engine;
	GraphicsEngine::m_engine = nullptr;
}
