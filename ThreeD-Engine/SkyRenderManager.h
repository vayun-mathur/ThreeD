#pragma once
#include "prerequisites.h"
#include "AppWindow.h"

class SkyRenderManager {
public:
	void init();
	void render(CameraObjectPtr cam);
	void render_stars(ConstantBufferPtr cb, constant& cc, CameraObjectPtr cam);
private:
	void CreateCube();
private:
	VertexShaderPtr m_vs_sky, m_vs_stars;
	PixelShaderPtr m_ps_sky, m_ps_stars;

	VertexBufferPtr m_cubeVB;
	IndexBufferPtr m_cubeIB;

	ConstantBufferPtr m_cb;
	TexturePtr texture;
	MeshPtr m_mesh;
};