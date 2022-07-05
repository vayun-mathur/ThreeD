#include "GUIRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "Frame.h"
#include "GUIRect.h"

__declspec(align(16))
struct buf___ {
	vec2 center;
	vec2 size;
	vec4 color;
	float z;
	float radius;
};

buf___ b;

void GUIRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"GUIShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"GUIShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&b, sizeof(buf___));

	std::vector<vec2> list_vertices = { vec2(-1, -1), vec2(1, -1), vec2(1, 1), vec2(-1, 1), vec2(-1, -1), vec2(1, 1), vec2(0, 0)};

	shader_byte_code = nullptr;
	size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader, 4);
	m_vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(vec2),
		list_vertices.size(), shader_byte_code, size_shader, 4);
}

void GUIRenderManager::render(std::vector<FramePtr>& guis)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb, 0);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	for (FramePtr _gui : guis) {
		if (_gui->getType() == FrameType::GUIRect) {
			auto gui = std::reinterpret_pointer_cast<GUIRect>(_gui);

			FrameData d = gui->getRealCoordinates();
			b.center = d.position;
			b.size = d.scale;
			b.z = d.z;
			b.color = gui->color;
			b.radius = gui->radius;
			m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &b);

			//GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, texture, index);

			GraphicsEngine::get()->getRenderSystem()->setRasterizerState(CULL_MODE::FRONT);
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleList(6, 0);
			GraphicsEngine::get()->getRenderSystem()->setRasterizerState(CULL_MODE::BACK);
		}
	}
}
