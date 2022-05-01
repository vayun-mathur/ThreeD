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

__declspec(align(16))
struct MatrixBuffer
{
	mat4 transform;
	mat4 view;
	mat4 projection;
};

MatrixBuffer buf;

__declspec(align(16))
struct WindowSizeBuffer
{
	float windowSize[2];
};

WindowSizeBuffer wbuf;
void CreateCube();
void LoadVolume(ID3D11Device* const device, const LPCWSTR file);

//volume texture
ID3D11Texture3D* m_volumeTex3D;
ID3D11ShaderResourceView* m_volRSV;
//vertex and index buffers
VertexBufferPtr m_cubeVB;
IndexBufferPtr m_cubeIB;
ConstantBufferPtr m_cb;
ConstantBufferPtr m_wcb;
FrameBufferPtr m_cubeFront, m_cubeBack;

ID3D11RasterizerState* m_backFaceCull, * m_frontFaceCull;

const UINT g_iVolumeSize = 256;	// voxel volume width, height and depth
const UINT g_vol = 178;

void Initialize(ID3D11Device* const device, const int width, const int height)
{

	// create a resource view (RT) for the front and back of the volume
	m_cubeFront = GraphicsEngine::get()->getRenderSystem()->createFrameBuffer(width, height);
	m_cubeBack = GraphicsEngine::get()->getRenderSystem()->createFrameBuffer(width, height);

	// load of the raw textures into a D3D11_TEXTURE3D_DESC 
	LoadVolume(device, L"./Assets/foot.raw");

	// create the volume/cube primitive
	CreateCube();

	buf.view = AppWindow::s_main->m_scene->getCamera()->getViewMatrix();
	buf.projection = AppWindow::s_main->m_scene->getCamera()->getProjectionMatrix();
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&buf, sizeof(MatrixBuffer));

	wbuf.windowSize[0] = 1.f / width;
	wbuf.windowSize[1] = 1.f / height;
	m_wcb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&wbuf, sizeof(WindowSizeBuffer));
}


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
		m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &buf);

		//-----------------------------------------------------------------------------//
		// Back and front buffer for faces of the volume
		//-----------------------------------------------------------------------------//

		// Set the vertex shader ~ simple model shader
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_cube_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_cube_vs, m_cb, 0);

		// Set the pixel shader ~ simple model shader
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_cube_ps);

		deviceContext->RSSetState(m_backFaceCull);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(m_cubeBack, 0.f, 0.f, 0.f, 1.f);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(36, 0, 0);

		// Back-face culling
		deviceContext->RSSetState(m_frontFaceCull);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(m_cubeFront, 0.f, 0.f, 0.f, 1.f);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(36, 0, 0);

		//-----------------------------------------------------------------------------//
		// Ray-casting / Volume Rendering 
		//-----------------------------------------------------------------------------//


		// Render to standard render target
		deviceContext->OMSetRenderTargets(1, &AppWindow::s_main->m_swap_chain->m_rtv, NULL);

		// Set the vertex shader to the Volume Renderer vertex program
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_ray_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_vs, m_cb, 1);

		// Set the pixel shader to the Volume Render pixel program
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ray_ps);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_ps, m_wcb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ray_ps, m_cb, 1);

		// pass in our textures )
		deviceContext->PSSetShaderResources(0, 1, &m_volRSV); // the loaded RAW file
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, m_cubeFront->getTexture(), 1);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ray_ps, m_cubeBack->getTexture(), 2);

		// Draw the cube
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(36, 0, 0);

		// Un-bind textures
		ID3D11ShaderResourceView* nullRV[3] = { NULL, NULL, NULL };
		deviceContext->PSSetShaderResources(0, 3, nullRV);
	}
}

void Shutdown()
{

	if (m_volumeTex3D != nullptr) {
		m_volumeTex3D->Release();
		m_volumeTex3D = nullptr;
	}

	if (m_volRSV != nullptr) {
		m_volRSV->Release();
		m_volRSV = nullptr;
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

//---------------------------------------------------------------//
// Load RAW texture files 
//---------------------------------------------------------------//
void LoadVolume(ID3D11Device* const device, const LPCWSTR file)
{
	HRESULT hr;
	HANDLE hFile = CreateFileW(file, GENERIC_READ, 0, NULL, OPEN_EXISTING, OPEN_EXISTING, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Opening volume data file failed.", L"Error", MB_ICONERROR | MB_OK);

	}
	BYTE* buffer = (BYTE*)malloc(g_iVolumeSize * g_iVolumeSize * g_iVolumeSize * sizeof(BYTE));

	DWORD numberOfBytesRead = 0;
	if (ReadFile(hFile, buffer, g_iVolumeSize * g_iVolumeSize * g_iVolumeSize, &numberOfBytesRead, NULL) == 0)
	{
		MessageBox(NULL, L"Reading volume data failed.", L"Error", MB_ICONERROR | MB_OK);
	}

	CloseHandle(hFile);

	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = g_iVolumeSize;
	descTex.Width = g_iVolumeSize;
	descTex.Depth = g_iVolumeSize;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8_UNORM;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	// Initial data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = buffer;
	initData.SysMemPitch = g_iVolumeSize;
	initData.SysMemSlicePitch = g_iVolumeSize * g_iVolumeSize;
	// Create texture
	hr = (device->CreateTexture3D(&descTex, &initData, &m_volumeTex3D));

	// Create a resource view of the texture
	hr = (device->CreateShaderResourceView(m_volumeTex3D, NULL, &m_volRSV));

	free(buffer);
}

void VolumetricRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"./Assets/model_position.hlsl", "ModelPositionVS", &shader_byte_code, &size_shader);
	m_cube_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"./Assets/model_position.hlsl", "ModelPositionPS", &shader_byte_code, &size_shader);
	m_cube_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"./Assets/raycast.hlsl", "RayCastVS", &shader_byte_code, &size_shader);
	m_ray_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"./Assets/raycast.hlsl", "RayCastPS", &shader_byte_code, &size_shader);
	m_ray_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rasterizerDesc, &m_backFaceCull);

	// Front face culling
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rasterizerDesc, &m_frontFaceCull);

	Initialize(GraphicsEngine::get()->getRenderSystem()->m_d3d_device, AppWindow::s_main->getScreenSize().right, AppWindow::s_main->getScreenSize().bottom);
}

void VolumetricRenderManager::render(std::vector<VolumeObjectPtr>& volumes)
{
	Render(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context, volumes);
}
