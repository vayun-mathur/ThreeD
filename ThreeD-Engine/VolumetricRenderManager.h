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
	void update();
private:
	void Render(ID3D11DeviceContext* const deviceContext, std::vector<VolumeObjectPtr>& volumes);
	void updateWorleyPointsBuffer(StructuredBufferPtr buffer, int numCellsPerAxis);
	void updateCloudShapeTex(RWTexture3DPtr tex, unsigned int resolution, vec4 mask);
	void genWeatherMap();
private:
	VertexShaderPtr m_ray_vs;
	PixelShaderPtr m_ray_ps;
};

