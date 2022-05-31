#pragma once
#include <d3d11.h>
#include "prerequisites.h"
#include <vector>
#include "Vector.h"

class VolumetricRenderManager
{
public:
	void init();
	void render(std::vector<VolumeObjectPtr>& volumes);
private:
	void Render(ID3D11DeviceContext* const deviceContext, std::vector<VolumeObjectPtr>& volumes);
private:
	VertexShaderPtr m_ray_vs;
	PixelShaderPtr m_ray_ps;
};

