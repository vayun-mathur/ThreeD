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

__declspec(align(16))
struct MatrixBuffer
{
	mat4 transform;
	mat4 view;
	mat4 projection;
	mat4 inv_transform;
	vec3 cam_box_coords;
	float f;
	vec3 bounds_min;
	float f2;
	vec3 bounds_max;
	float f3;
	vec3 cam_pos;
	float f4;
	vec3 cam_dir;
	float f5;
};

MatrixBuffer buf;

void CreateCube();

Texture3DPtr m_vol;
VertexBufferPtr m_cubeVB;
IndexBufferPtr m_cubeIB;
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
		buf.transform.setScale(volume->getScale());
		buf.transform.setTranslation(volume->getPosition());
		vec3 min = buf.transform(vec4(-1, -1, -1, 1)).xyz();
		vec3 max = buf.transform(vec4(1, 1, 1, 1)).xyz();
		vec3 p = AppWindow::s_main->m_scene->getCamera()->getCameraPosition() - min;
		vec3 s = max - min;
		buf.cam_box_coords = vec3(p.x / s.x, p.y / s.y, p.z / s.z);
		buf.bounds_min = min;
		buf.bounds_max = max;
		buf.cam_pos = AppWindow::s_main->m_scene->getCamera()->getCameraPosition();
		buf.cam_dir = buf.view(vec4(0, 0, 1, 0)).xyz();
		buf.inv_transform = buf.transform;
		buf.inv_transform.inverse();
		m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &buf);

		//-----------------------------------------------------------------------------//
		// Ray-casting / Volume Rendering 
		//-----------------------------------------------------------------------------//


		// Render to standard render target
		deviceContext->RSSetState(m_frontFaceCull);
		deviceContext->OMSetRenderTargets(1, &AppWindow::s_main->m_swap_chain->m_rtv, NULL);

		// set shaders
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_ray_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ray_ps);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_vs, m_cb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_ps, m_cb, 0);

		// pass in our textures )
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, m_vol, 0);

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


void VolumetricRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"./Assets/raycast.hlsl", "RayCastVS", &shader_byte_code, &size_shader);
	m_ray_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"./Assets/raycast.hlsl", "RayCastPS", &shader_byte_code, &size_shader);
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

	// create the volume/cube primitive
	CreateCube();

	buf.view = AppWindow::s_main->m_scene->getCamera()->getViewMatrix();
	buf.projection = AppWindow::s_main->m_scene->getCamera()->getProjectionMatrix();
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(MatrixBuffer));
}

void VolumetricRenderManager::render(std::vector<VolumeObjectPtr>& volumes)
{
	Render(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context, volumes);
}
