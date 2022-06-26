#include "VolumetricRenderManager.h"
#include <Windows.h>

#include <DirectXMath.h>
#include <d3d11.h>

#include "GraphicsEngine.h"
#include "AppWindow.h"
#include "CameraObject.h"
#include "VolumeObject.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Texture3D.h"
#include "ComputeShader.h"
#include "StructuredBuffer.h"
#include "RWTexture3D.h"
#include "RWTexture.h"

__declspec(align(16))
struct MatrixBuffer
{
	mat4 transform;
	mat4 view;
	mat4 projection;
	mat4 inv_transform;
	vec3 cam_box_coords;
	float lightAbsorptionThroughCloud = 0.85;
	vec3 bounds_min;
	float lightAbsorptionTowardSun = 0.94;
	vec3 bounds_max;
	float f3;
	vec3 cam_pos;
	float f4;
	vec3 cam_dir;
	float f5;
	vec2 screen_size;
};

__declspec(align(16))
struct CloudBuffer
{
	vec4 shapeNoiseWeights = vec4(2.5, 0.65, 0.2, 0.25);
	vec3 shapeOffset = vec3(0, 0, 0);
	float scale = 1;
	vec3 detailOffset = vec3(0, 0, 0);
	float detailNoiseScale = 3;
	vec3 detailWeights = vec3(1, 0.5, 0.5);
	float detailNoiseWeight = 0;
	float densityOffset = -1;
	float densityMultiplier = 1;
	float timeScale = 1;
	float baseSpeed = 0.5;
	float detailSpeed = 1;
	float rayOffsetStrength = 0.05;
};

MatrixBuffer buf;
CloudBuffer cbuf;

void CreateCube();

Texture3DPtr m_vol;
TexturePtr m_blue;
VertexBufferPtr m_cubeVB;
IndexBufferPtr m_cubeIB;
ConstantBufferPtr m_mb;
ConstantBufferPtr m_cb;

ID3D11RasterizerState* m_frontFaceCull;


void VolumetricRenderManager::Render(ID3D11DeviceContext* const deviceContext, std::vector<VolumeObjectPtr>& volumes)
{
	for (VolumeObjectPtr volume : volumes) {

		// Set vertex buffer
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_cubeVB);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_cubeIB);

		//----------------------------------------------------------------------------//
		// Create our MVP transforms 
		//-----------------------------------------------------------------------------//

		buf.projection = AppWindow::s_main->m_scene->getCamera()->getProjectionMatrix();
		buf.view = AppWindow::s_main->m_scene->getCamera()->getViewMatrix();
		buf.transform = mat4();
		vec3 tmin = volume->getMin();// min(volume->getMin(), AppWindow::s_main->m_scene->getCamera()->getCameraPosition() - 2);
		vec3 tmax = volume->getMax();//max(volume->getMax(), AppWindow::s_main->m_scene->getCamera()->getCameraPosition() + 2);
		buf.transform.setTranslation(tmin + (tmax - tmin)/2);
		buf.transform.setScale((tmax - tmin)/2);

		vec3 min = volume->getMin();
		vec3 max = volume->getMax();
		vec3 p = AppWindow::s_main->m_scene->getCamera()->getCameraPosition() - min;
		vec3 s = max - min;
		buf.cam_box_coords = vec3(p.x / s.x, p.y / s.y, p.z / s.z);
		buf.bounds_min = min;
		buf.bounds_max = max;
		buf.cam_pos = AppWindow::s_main->m_scene->getCamera()->getCameraPosition();
		buf.cam_dir = buf.view(vec4(0, 0, 1, 0)).xyz();
		buf.inv_transform = mat4();
		buf.inv_transform.setTranslation(min + (max - min) / 2);
		buf.inv_transform.setScale((max - min) / 2);
		buf.inv_transform.inverse();
		buf.screen_size = vec2(AppWindow::s_main->getClientWindowRect().right- AppWindow::s_main->getClientWindowRect().left, AppWindow::s_main->getClientWindowRect().bottom - AppWindow::s_main->getClientWindowRect().top);
		m_mb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &buf);

		//-----------------------------------------------------------------------------//
		// Ray-casting / Volume Rendering 
		//-----------------------------------------------------------------------------//


		// Render to standard render target
		deviceContext->RSSetState(m_frontFaceCull);
		deviceContext->OMSetRenderTargets(1, &AppWindow::s_main->m_swap_chain->m_rtv, NULL);

		// set shaders
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_ray_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ray_ps);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_vs, m_mb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_ps, m_mb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_vs, m_cb, 1);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_ps, m_cb, 1);

		// pass in our textures )
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, m_vol, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, m_blue, 1);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, AppWindow::s_main->everything->getDepthTexture(), 2);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, AppWindow::s_main->everything->getTexture(), 3);

		// Draw the cube
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(36, 0, 0);
	}
}

//---------------------------------------------------------------//
// Create a 3d Cube vertext object
// In order to render the texture (volume) 
// fit a bounding box to the texture.
//---------------------------------------------------------------//
void CreateCube()
{
	// Create ray-cast vertex and index buffers
	HRESULT hr = S_OK;
	vec3 vertices[] =
	{
		vec3(-1.f, -1.f, -1.f),
		vec3(-1.f, -1.f, 1.f),
		vec3(-1.f, 1.f, -1.f),
		vec3(-1.f, 1.f, 1.f),
		vec3(1.f, -1.f, -1.f),
		vec3(1.f, -1.f, 1.f),
		vec3(1.f, 1.f, -1.f),
		vec3(1.f, 1.f, 1.f),
	};

	// Create index buffer
	UINT indices[] =
	{
		0, 1, 2,
		2, 1, 3,

		0, 4, 1,
		1, 4, 5,

		0, 2, 4,
		4, 2, 6,

		1, 5, 3,
		3, 5, 7,

		2, 3, 6,
		6, 3, 7,

		5, 4, 7,
		7, 4, 6,
	};

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 3);
	m_cubeVB = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&vertices[0], sizeof(vec3),
		8, shader_byte_code, size_shader, 3);
	m_cubeIB = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], 36);
}

float randomFloat() {
	return (float)rand() / RAND_MAX;
}

void VolumetricRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"CloudShader.hlsl", "RayCastVS", &shader_byte_code, &size_shader);
	m_ray_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"CloudShader.hlsl", "RayCastPS", &shader_byte_code, &size_shader);
	m_ray_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	// Front face culling
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rasterizerDesc, &m_frontFaceCull);

	// load of the raw textures into a D3D11_TEXTURE3D_DESC 
	m_vol = std::make_shared<Texture3D>(L"./Assets/cloud.raw", 256);
	m_blue = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"./Assets/Textures/LDR_LLL1_0.png");

	// create the volume/cube primitive
	CreateCube();

	buf.view = AppWindow::s_main->m_scene->getCamera()->getViewMatrix();
	buf.projection = AppWindow::s_main->m_scene->getCamera()->getProjectionMatrix();
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(CloudBuffer));
	m_mb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(MatrixBuffer));
}

void VolumetricRenderManager::render(std::vector<VolumeObjectPtr>& volumes)
{
	Render(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context, volumes);
}