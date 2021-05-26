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
	try {
		m_material_manager = new MaterialManager();
	}
	catch (...) { throw std::exception("MaterialManager not created successfully"); }

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	m_render_system->compileVertexShader(L"VertexMeshLayoutShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	::memcpy(m_mesh_layout_byte_code, shader_byte_code, size_shader);
	m_mesh_layout_size = size_shader;
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

MaterialManager* GraphicsEngine::getMaterialManager()
{
	return m_material_manager;
}

void GraphicsEngine::getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size)
{
	*byte_code = m_mesh_layout_byte_code;
	*size = m_mesh_layout_size;
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
