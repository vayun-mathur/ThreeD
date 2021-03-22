#pragma once
#include <d3d11.h>
#include "prerequisites.h"
#include "RenderSystem.h"

class GraphicsEngine
{
public:
	//Initialize the GraphicsEngine and DirectX 11 Device
	GraphicsEngine();

	RenderSystem* getRenderSystem();

	//Release all the resources loaded
	~GraphicsEngine();
public:
	static GraphicsEngine* get();
	static void create();
	static void release();
private:
	RenderSystem* m_render_system = nullptr;
	static GraphicsEngine* m_engine;
};

