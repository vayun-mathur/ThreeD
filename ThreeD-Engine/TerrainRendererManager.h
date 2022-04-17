#pragma once
#include "prerequisites.h"
#include <vector>

struct constant;

class TerrainRenderManager
{
public:
	void init();
	void render(std::vector<TerrainObjectPtr>& terrains, ConstantBufferPtr cb, constant& cc);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
};

