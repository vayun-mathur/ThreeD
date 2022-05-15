#pragma once
#include "prerequisites.h"
#include <vector>
#include <Windows.h>

struct constant;

class WaterRenderManager
{
public:
	void init(RECT rc);
	void render(std::vector<WaterTileObjectPtr>& waters, ConstantBufferPtr cb, constant& cc);
	void update(double dt);

	FrameBufferPtr getReflectionTexture() { return m_reflection; }
	FrameBufferPtr getRefractionTexture() { return m_refraction; }
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	VertexBufferPtr m_vertex_buffer;
	ConstantBufferPtr m_wcb;
	FrameBufferPtr m_reflection, m_refraction;
};

