#include "Texture3D.h"
#include <DirectXTex.h>
#include "GraphicsEngine.h"

Texture3D::Texture3D(const wchar_t* full_path, int size) : Resource(full_path)
{
	HRESULT hr;
	HANDLE hFile = CreateFileW(full_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, OPEN_EXISTING, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Opening volume data file failed.", L"Error", MB_ICONERROR | MB_OK);

	}
	BYTE* buffer = (BYTE*)malloc(size * size * size * sizeof(BYTE));

	DWORD numberOfBytesRead = 0;
	if (ReadFile(hFile, buffer, size * size * size, &numberOfBytesRead, NULL) == 0)
	{
		MessageBox(NULL, L"Reading volume data failed.", L"Error", MB_ICONERROR | MB_OK);
	}

	CloseHandle(hFile);

	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = size;
	descTex.Width = size;
	descTex.Depth = size;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8_UNORM;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	// Initial data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = buffer;
	initData.SysMemPitch = size;
	initData.SysMemSlicePitch = size * size;
	// Create texture
	hr = (GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateTexture3D(&descTex, &initData, &m_texture));

	// Create a resource view of the texture
	hr = (GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture, NULL, &m_shader_res_view));

	free(buffer);
}

Texture3D::Texture3D(ID3D11Texture3D* texture, ID3D11ShaderResourceView* shader_res_view)
	: Resource(L""), m_texture(texture), m_shader_res_view(shader_res_view)
{
}

Texture3D::~Texture3D()
{
	m_shader_res_view->Release();
	m_texture->Release();
}
