#pragma once
#include "prerequisites.h"
#include <vector>

struct constant;

class MeshRenderManager
{
public:
	void init();
	void render(std::vector<MeshObjectPtr>& meshes, ConstantBufferPtr cb, constant& cc);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
};

