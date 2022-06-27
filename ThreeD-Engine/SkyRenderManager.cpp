#include "SkyRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "AppWindow.h"
#include "CameraObject.h"
#include "Mesh.h"


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
	for (int i = 0; i < 36; i += 3) {
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
	m_vs_sky = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps_sky = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SkyboxShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs_stars = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyboxShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps_stars = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	CreateCube();
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&b, sizeof(_buf__));

	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets/Meshes/sphere.obj");
	texture = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/stars_map.jpg");
}

void SkyRenderManager::render(CameraObjectPtr cam)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_cubeVB);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_cubeIB);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs_sky);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps_sky);
	double t = (float)::GetTickCount64() / 5000;
	//t = 0.05;
	b.sun_dir = vec3(0, sin(t), cos(t));
	b.transform.setIdentity();
	b.transform.setTranslation(cam->getCameraPosition());
	b.transform.setScale(vec3(2000));
	b.campos = cam->getCameraPosition();
	b.view = cam->getViewMatrix();
	b.projection = cam->getProjectionMatrix();
	b.screenSize = vec2(AppWindow::s_main->getClientWindowRect().right - AppWindow::s_main->getClientWindowRect().left, AppWindow::s_main->getClientWindowRect().bottom - AppWindow::s_main->getClientWindowRect().top);
	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &b);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs_sky, m_cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps_sky, m_cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_sky, AppWindow::s_main->everything->getDepthTexture(), 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_sky, AppWindow::s_main->everything->getTexture(), 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_sky, AppWindow::s_main->stars->getTexture(), 2);

	// Draw the cube
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(36, 0, 0);
}

void SkyRenderManager::render_stars(ConstantBufferPtr cb, constant& cc, CameraObjectPtr cam)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs_stars);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps_stars);


	cc.m_transform.setIdentity();
	cc.m_transform.setTranslation(cam->getCameraPosition());
	cc.m_transform.setScale(vec3(100));
	cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs_stars, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps_stars, cb, 0);


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());

	for (MaterialIndexRange mir : m_mesh->getMaterials()) {
		MaterialPtr material = mir.material;

		//SET MATERIAL
		GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs_stars, material->getConstantBuffer(), 1);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps_stars, material->getConstantBuffer(), 1);

		for (auto&& [index, texture] : material->getTextures()) {
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_stars, texture, index);
		}
		GraphicsEngine::get()->getRenderSystem()->setRasterizerState(CULL_MODE::FRONT);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_stars, texture, 0);

		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mir.high - mir.low, 0, mir.low);
		GraphicsEngine::get()->getRenderSystem()->setRasterizerState(CULL_MODE::BACK);
	}
}