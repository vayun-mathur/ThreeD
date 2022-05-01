#pragma once
#include <d3d11.h>

class VolumetricRenderManager
{
public:
	void init();
	void render();
private:
	void Render(ID3D11DeviceContext* const deviceContext, ID3D11RasterizerState* const back, ID3D11RasterizerState* const front, ID3D11RenderTargetView* const rtView);
};

