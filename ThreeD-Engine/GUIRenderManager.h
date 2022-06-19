#pragma once
#include "prerequisites.h"
#include <vector>
#include "vector.h"

struct GUI {
	TexturePtr tex;
	vec2 center;
	vec2 size;
	float radius;
};

class GUIRenderManager
{
public:
	void init();
	void render(std::vector<GUI>& guis);
private:
	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;

	ConstantBufferPtr m_cb;
	VertexBufferPtr m_vb;
};

