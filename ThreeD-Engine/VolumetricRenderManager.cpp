#include "VolumetricRenderManager.h"
#include <Windows.h>

#include <DirectXMath.h>
#include <d3d11.h>
#include "Model.h"
#include "RayCastMaterial.h"

#include "GraphicsEngine.h"
#include "AppWindow.h"

struct MatrixBuffer
{
	DirectX::XMMATRIX mWVP;
};

void CreateRenderTexture(ID3D11Device* const device, const int width, const int height);
void CreateSampler(ID3D11Device* const device);
void CreateCube(ID3D11Device* const device);
void LoadVolume(ID3D11Device* const device, const LPCWSTR file);

// rotation - y-axis (super lazy but I only want to rotate it on this :P)
float rot = 1;

// "materials"
Model* m_modelShader;
RayCastMaterial* m_volumeRaycastShader;

//render textures
ID3D11Texture2D* m_modelTex2DFront;
ID3D11ShaderResourceView* m_modelSRVFront;
ID3D11RenderTargetView* m_modelRTVFront;
ID3D11Texture2D* m_modelText2DBack;
ID3D11ShaderResourceView* m_modelRSVBack;
ID3D11RenderTargetView* m_ModelRTVBack;
//sampler 
ID3D11SamplerState* m_samplerLinear;
//volume texture
ID3D11Texture3D* m_volumeTex3D;
ID3D11ShaderResourceView* m_volRSV;
//vertex and index buffers
ID3D11Buffer* m_cubeVB;
ID3D11Buffer* m_cubeIB;

// project matrix for render
DirectX::XMFLOAT4X4 viewProj;

const UINT g_iVolumeSize = 256;	// voxel volume width, height and depth
const UINT g_vol = 178;

void Initialize(ID3D11Device* const device, const int width, const int height)
{
	// set up the shader/"material" to render the cube (the volume which the texture will mapped too)
	m_modelShader = new Model;
	m_modelShader->Initialize(device);

	// set up the volume ray cast shader (like above, this is just a wrapper for compiling shader with DX)
	m_volumeRaycastShader = new RayCastMaterial;
	m_volumeRaycastShader->Initialize(device, width, height);

	// create a resource view (RT) for the front and back of the volume
	CreateRenderTexture(device, width, height);

	// set up simple linear sampler for use within our PS
	CreateSampler(device);

	// load of the raw textures into a D3D11_TEXTURE3D_DESC 
	LoadVolume(device, L"./Assets/foot.raw");

	// create the volume/cube primitive
	CreateCube(device);

	// Initialize the view matrix
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.f, 1.5f, -5.0f, 0.f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.f, 0.0f, 0.f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMMATRIX mView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, at, up));

	// Initialize the projection matrix
	DirectX::XMMATRIX mProjection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 1.f, 0.1f, 10.f));

	// View-projection matrix	
	XMStoreFloat4x4(&viewProj, XMMatrixMultiply(mProjection, mView));
}

void Update(ID3D11Device* const device, const float dt)
{
	// rotate rendered volume around y-axis (oo so fancy :P)
	rot += 1.2f * dt;
	/*
	// Note: this *does* really suck and please forgive me 
	if (InputManager::Instance()->IsKeyDown(DIK_1))
	{
		LoadVolume(device, L"../VolumeRenderer/aneurism.raw");
	}

	if (InputManager::Instance()->IsKeyDown(DIK_2))
	{
		LoadVolume(device, L"../VolumeRenderer/skull.raw");
	}

	if (InputManager::Instance()->IsKeyDown(DIK_3))
	{
		LoadVolume(device, L"../VolumeRenderer/bonsai.raw");
	}

	if (InputManager::Instance()->IsKeyDown(DIK_4))
	{
		LoadVolume(device, L"../VolumeRenderer/foot.raw");
	}
	*/
}

void VolumetricRenderManager::Render(ID3D11DeviceContext* const deviceContext, ID3D11RasterizerState* const back, ID3D11RasterizerState* const front, ID3D11RenderTargetView* const rtView)
{
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };

	// Set vertex buffer
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_cubeVB, &stride, &offset);
	// Set index buffer
	deviceContext->IASetIndexBuffer(m_cubeIB, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the input layout
	deviceContext->IASetInputLayout(m_modelShader->GetInputLayout());

	//----------------------------------------------------------------------------//
	// Create our MVP transforms 
	//-----------------------------------------------------------------------------//
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(1, 1, 1);
	DirectX::XMMATRIX m_rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0, rot, 0);
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(0, 0, 0);

	DirectX::XMMATRIX m_newWorld = scaleMat * m_rotationMatrix * transMat;

	DirectX::XMMATRIX m_worldMatrix = m_newWorld;

	MatrixBuffer cb;
	cb.mWVP = XMMatrixMultiply(XMLoadFloat4x4(&viewProj), m_worldMatrix);
	deviceContext->UpdateSubresource(m_modelShader->m_MatrixBuffer, 0, NULL, &cb, 0, 0);

	//-----------------------------------------------------------------------------//
	// Back and front buffer for faces of the volume
	//-----------------------------------------------------------------------------//

	// Set the vertex shader ~ simple model shader
	deviceContext->VSSetShader(m_modelShader->GetVertexShader(), NULL, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &m_modelShader->m_MatrixBuffer);

	// Set the pixel shader ~ simple model shader
	deviceContext->PSSetShader(m_modelShader->GetPixelShader(), NULL, 0);

	deviceContext->RSSetState(back);
	deviceContext->ClearRenderTargetView(m_ModelRTVBack, clearColor);
	deviceContext->OMSetRenderTargets(1, &m_ModelRTVBack, NULL);
	deviceContext->DrawIndexed(36, 0, 0);		// Draw back faces

	// Back-face culling
	deviceContext->RSSetState(front);
	deviceContext->ClearRenderTargetView(m_modelRTVFront, clearColor);
	deviceContext->OMSetRenderTargets(1, &m_modelRTVFront, NULL);
	deviceContext->DrawIndexed(36, 0, 0);		// Draw front faces

	//-----------------------------------------------------------------------------//
	// Ray-casting / Volume Rendering 
	//-----------------------------------------------------------------------------//

	// Set the input layout
	deviceContext->IASetInputLayout(m_modelShader->GetInputLayout());

	// Render to standard render target
	deviceContext->OMSetRenderTargets(1, &AppWindow::s_main->m_swap_chain->m_rtv, NULL);

	// Set the vertex shader to the Volume Renderer vertex program
	deviceContext->VSSetShader(m_volumeRaycastShader->GetVertexShader(), NULL, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &m_modelShader->m_MatrixBuffer);

	// Set the pixel shader to the Volume Render pixel program
	deviceContext->PSSetShader(m_volumeRaycastShader->GetPixelShader(), NULL, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &m_volumeRaycastShader->m_WindowSizeCB);

	// Set texture sampler
	deviceContext->PSSetSamplers(0, 1, &m_samplerLinear);

	// pass in our textures )
	deviceContext->PSSetShaderResources(0, 1, &m_volRSV); // the loaded RAW file
	deviceContext->PSSetShaderResources(1, 1, &m_modelSRVFront); // the front facing RT 
	deviceContext->PSSetShaderResources(2, 1, &m_modelRSVBack); // the back facing RT

	// Draw the cube
	deviceContext->DrawIndexed(36, 0, 0);

	// Un-bind textures
	ID3D11ShaderResourceView* nullRV[3] = { NULL, NULL, NULL };
	deviceContext->PSSetShaderResources(0, 3, nullRV);
}

void Shutdown()
{
	// release all our resources
	if (m_modelTex2DFront != nullptr) {
		m_modelTex2DFront->Release();
		m_modelTex2DFront = nullptr;
	}

	if (m_modelSRVFront != nullptr) {
		m_modelSRVFront->Release();
		m_modelSRVFront = nullptr;
	}

	if (m_modelRTVFront != nullptr) {
		m_modelRTVFront->Release();
		m_modelRTVFront = nullptr;
	}

	if (m_modelText2DBack != nullptr) {
		m_modelText2DBack->Release();
		m_modelText2DBack = nullptr;
	}

	if (m_modelRSVBack != nullptr) {
		m_modelRSVBack->Release();
		m_modelRSVBack = nullptr;
	}

	if (m_ModelRTVBack != nullptr) {
		m_ModelRTVBack->Release();
		m_ModelRTVBack = nullptr;
	}

	if (m_samplerLinear != nullptr) {
		m_samplerLinear->Release();
		m_samplerLinear = nullptr;
	}

	if (m_volumeTex3D != nullptr) {
		m_volumeTex3D->Release();
		m_volumeTex3D = nullptr;
	}

	if (m_volRSV != nullptr) {
		m_volRSV->Release();
		m_volRSV = nullptr;
	}

	if (m_cubeVB != nullptr) {
		m_cubeVB->Release();
		m_cubeVB = nullptr;
	}

	if (m_cubeIB != nullptr) {
		m_cubeIB->Release();
		m_cubeIB = nullptr;
	}
}

//---------------------------------------------------------------//
// Create a 3d Cube vertext object
// In order to render the texture (volume) 
// fit a bounding box to the texture.
//---------------------------------------------------------------//
void CreateCube(ID3D11Device* const device)
{
	// Create ray-cast vertex and index buffers
	HRESULT hr = S_OK;
	DirectX::XMFLOAT3 vertices[] =
	{
		DirectX::XMFLOAT3(-1.f, -1.f, -1.f),
		DirectX::XMFLOAT3(-1.f, -1.f, 1.f),
		DirectX::XMFLOAT3(-1.f, 1.f, -1.f),
		DirectX::XMFLOAT3(-1.f, 1.f, 1.f),
		DirectX::XMFLOAT3(1.f, -1.f, -1.f),
		DirectX::XMFLOAT3(1.f, -1.f, 1.f),
		DirectX::XMFLOAT3(1.f, 1.f, -1.f),
		DirectX::XMFLOAT3(1.f, 1.f, 1.f),
	};
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DirectX::XMFLOAT3) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	hr = (device->CreateBuffer(&bd, &initData, &m_cubeVB));

	// Create index buffer
	WORD indices[] =
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
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indices;
	hr = (device->CreateBuffer(&bd, &initData, &m_cubeIB));
}

//---------------------------------------------------------------//
// Create a RT for the screen
//---------------------------------------------------------------//
void CreateRenderTexture(ID3D11Device* const device, const int width, const int height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.ArraySize = 1;
	descTex.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descTex.Width = width;//m_D3D->m_ScreenWidth;
	descTex.Height = height;
	descTex.MipLevels = D3D11_BIND_VERTEX_BUFFER;
	descTex.SampleDesc.Count = 1;
	descTex.CPUAccessFlags = 0;

	// front facing RT
	hr = device->CreateTexture2D(&descTex, NULL, &m_modelTex2DFront);
	// Create resource view
	hr = device->CreateShaderResourceView(m_modelTex2DFront, NULL, &m_modelSRVFront);
	// Create render target view
	hr = device->CreateRenderTargetView(m_modelTex2DFront, NULL, &m_modelRTVFront);

	// back facing RT
	(device->CreateTexture2D(&descTex, NULL, &m_modelText2DBack));
	// Create resource view
	hr = device->CreateShaderResourceView(m_modelText2DBack, NULL, &m_modelRSVBack);
	// Create render target view
	hr = device->CreateRenderTargetView(m_modelText2DBack, NULL, &m_ModelRTVBack);
}

//---------------------------------------------------------------//
// Set up sampler for volume renderer
//---------------------------------------------------------------//
void CreateSampler(ID3D11Device* const device)
{
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &m_samplerLinear);
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

ID3D11RasterizerState* m_backFaceCull, *m_frontFaceCull;

void VolumetricRenderManager::init()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;
	(GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rasterizerDesc, &m_backFaceCull));

	// Front face culling
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	(GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rasterizerDesc, &m_frontFaceCull));

	Initialize(GraphicsEngine::get()->getRenderSystem()->m_d3d_device, AppWindow::s_main->getScreenSize().right, AppWindow::s_main->getScreenSize().bottom);
}

void VolumetricRenderManager::render()
{
	Update(GraphicsEngine::get()->getRenderSystem()->m_d3d_device, 0.01);
	Render(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->m_device_context, m_backFaceCull, m_frontFaceCull, nullptr);
}
