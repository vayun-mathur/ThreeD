#pragma once
#include "prerequisites.h"
#include <vector>
#include "vector.h"

class GUIRenderManager
{
public:
	void init();
	void render(std::vector<FramePtr>& guis);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;

	ConstantBufferPtr m_cb;
	VertexBufferPtr m_vb;
};

