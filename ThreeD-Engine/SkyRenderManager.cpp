#include "SkyRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "AppWindow.h"
#include "CameraObject.h"



__declspec(align(16))
struct _buf__ {
	mat4 transform;
	mat4 view;
	mat4 projection;
	vec3 campos;
	float f;
	vec2 screenSize;
	float f2, f3;
	vec3 sun_dir;
};

_buf__ b;

void SkyRenderManager::CreateCube()
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
	for (int i = 0; i < 36; i+=3) {
		std::swap(indices[i], indices[i + 1]);
	}
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 5);
	m_cubeVB = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&vertices[0], sizeof(vec3),
		8, shader_byte_code, size_shader, 3);
	m_cubeIB = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], 36);
}

void SkyRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SkyShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	CreateCube();
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&b, sizeof(_buf__));
}

void SkyRenderManager::render(CameraObjectPtr cam)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_cubeVB);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_cubeIB);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);
	double t = (float)::GetTickCount64()/10000;
	b.sun_dir = vec3(0, sin(t), cos(t));
	b.transform.setIdentity();
	b.transform.setTranslation(cam->getCameraPosition());
	b.transform.setScale(vec3(2000));
	b.campos = cam->getCameraPosition();
	b.view = cam->getViewMatrix();
	b.projection = cam->getProjectionMatrix();
	b.screenSize = vec2(AppWindow::s_main->getClientWindowRect().right - AppWindow::s_main->getClientWindowRect().left, AppWindow::s_main->getClientWindowRect().bottom - AppWindow::s_main->getClientWindowRect().top);
	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &b);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb, 0);
	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, AppWindow::s_main->everything->getDepthTexture(), 0);
	//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, AppWindow::s_main->everything->getTexture(), 1);

	// Draw the cube
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(36, 0, 0);
}
