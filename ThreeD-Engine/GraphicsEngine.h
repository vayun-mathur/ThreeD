#pragma once
#include <d3d11.h>
#include "prerequisites.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"

class GraphicsEngine
{
public:
	//Initialize the GraphicsEngine and DirectX 11 Device
	GraphicsEngine();
	//Release all the resources loaded
	~GraphicsEngine();
public:
	RenderSystem* getRenderSystem();
	TextureManager* getTextureManager();
	MeshManager* getMeshManager();
	MaterialManager* getMaterialManager();

	void getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size);
public:
	static GraphicsEngine* get();
	static void create();
	static void release();
private:
	RenderSystem* m_render_system = nullptr;
	MeshManager* m_mesh_manager = nullptr;
	TextureManager* m_texture_manager = nullptr;
	MaterialManager* m_material_manager = nullptr;
	static GraphicsEngine* m_engine;

	unsigned char m_mesh_layout_byte_code[1024];
	size_t m_mesh_layout_size = 0;
};

