#pragma once
#include "prerequisites.h"
#include <vector>

struct constant;

class MeshRenderManager
{
public:
	void init();
	void render(std::vector<MeshObjectPtr>& meshes, ConstantBufferPtr cb, constant& cc);
	void render_skybox(MeshObjectPtr& skybox, ConstantBufferPtr cb, constant& cc);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	VertexShaderPtr m_vs_skybox;
	PixelShaderPtr m_ps_skybox;
	ConstantBufferPtr m_sc;
};

