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
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return m_render_system;
}

TextureManager* GraphicsEngine::getTextureManager()
{
	return m_texture_manager;
}

GraphicsEngine::~GraphicsEngine()
{
	GraphicsEngine::m_engine = nullptr;
	delete m_render_system;
	delete m_texture_manager;
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
