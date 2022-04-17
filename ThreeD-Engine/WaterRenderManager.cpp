#include "WaterRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "AppWindow.h"

__declspec(align(16))
struct water_constant
{
	float move_factor;
};

water_constant wc;

void WaterRenderManager::init(RECT rc)
{
	std::vector<WaterMesh> list_vertices = { WaterMesh(vec2(-1, -1)), WaterMesh(vec2(-1, 1)), WaterMesh(vec2(1, -1)),
		WaterMesh(vec2(1, -1)), WaterMesh(vec2(-1, 1)), WaterMesh(vec2(1, 1)) };

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 2);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(WaterMesh),
		list_vertices.size(), shader_byte_code, size_shader, 2);

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"WaterVertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"WaterPixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_wcb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&wc, sizeof(water_constant));

	m_reflection = std::make_shared<FrameBuffer>(rc.right - rc.left, rc.bottom - rc.top, GraphicsEngine::get()->getRenderSystem());
	m_refraction = std::make_shared<FrameBuffer>(rc.right - rc.left, rc.bottom - rc.top, GraphicsEngine::get()->getRenderSystem());
}

void WaterRenderManager::render(std::vector<WaterTileObjectPtr>& waters, ConstantBufferPtr cb, constant& cc)
{
	m_wcb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &wc);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_wcb, 1);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_reflection->getTexture(), 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_refraction->getTexture(), 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_refraction->getDepthTexture(), 4);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);

	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(CULL_MODE::BACK);

	for (WaterTileObjectPtr water : waters) {
		cc.m_transform.setIdentity();
		cc.m_transform.setTranslation(water->getPosition());
		cc.m_transform.setScale(water->getScale());
		cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
		
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, water->getDUDV(), 2);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, water->getNormalMap(), 3);

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleList(6, 0);
	}
}

void WaterRenderManager::update(double dt)
{
	wc.move_factor += 0.03 * dt;
	wc.move_factor -= floor(wc.move_factor);
}

void WaterRenderManager::setReflectionTexture()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_reflection, 0, 0, 0, 1);
}

void WaterRenderManager::setRefractionTexture()
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_refraction, 0, 0, 0, 1);
}
