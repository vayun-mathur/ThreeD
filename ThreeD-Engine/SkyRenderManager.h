#pragma once
#include "prerequisites.h"

class SkyRenderManager {
public:
	void init();
	void render(CameraObjectPtr cam);
private:
	void CreateCube();
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;

	VertexBufferPtr m_cubeVB;
	IndexBufferPtr m_cubeIB;

	ConstantBufferPtr m_cb;
};