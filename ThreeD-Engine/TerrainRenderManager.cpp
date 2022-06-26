#include "TerrainRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "AppWindow.h"
#include "TerrainObject.h"

TexturePtr tex;

void TerrainRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"./Assets/Textures/terrain.png");

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"TerrainShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"TerrainShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
}

void TerrainRenderManager::render(std::vector<TerrainObjectPtr>& terrains, ConstantBufferPtr cb, constant& cc)
{
	for (TerrainObjectPtr terrain : terrains) {
		cc.m_transform.setIdentity();
		cc.m_transform.setTranslation(terrain->getPosition());
		cc.m_transform.setScale(terrain->getScale());
		cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, tex, 0);

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(terrain->getMesh()->getVertexBuffer());
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(terrain->getMesh()->getIndexBuffer());

		for (MaterialIndexRange mir : terrain->getMesh()->getMaterials()) {
			MaterialPtr material = mir.material;

			GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, material->getConstantBuffer(), 1);
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, material->getConstantBuffer(), 1);

			for (auto&& [index, texture] : material->getTextures()) {
				GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, texture, index);
			}
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, AppWindow::s_main->sky_texture->getTexture(), 3);

			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mir.high - mir.low, 0, mir.low);
		}
	}
}
