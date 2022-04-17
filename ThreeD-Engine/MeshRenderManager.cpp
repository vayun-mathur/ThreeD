#include "MeshRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "AppWindow.h"
#include "MeshObject.h"

void MeshRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyboxShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps_skybox = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
}

void MeshRenderManager::render(std::vector<MeshObjectPtr>& meshes, ConstantBufferPtr cb, constant& cc)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);


	for (MeshObjectPtr mesh : meshes) {
		cc.m_transform.setIdentity();
		cc.m_transform.setTranslation(mesh->getPosition());
		cc.m_transform.setScale(mesh->getScale());
		cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);


		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getMesh()->getVertexBuffer());
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getMesh()->getIndexBuffer());

		for (MaterialIndexRange mir : mesh->getMesh()->getMaterials()) {
			MaterialPtr material = mir.material;

			//SET MATERIAL
			GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, material->getConstantBuffer(), 1);
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, material->getConstantBuffer(), 1);

			for (auto&& [index, texture] : material->getTextures()) {
				GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, texture, index);
			}


			// FINALLY DRAW THE TRIANGLE
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mir.high - mir.low, 0, mir.low);
		}
	}
}

void MeshRenderManager::render_skybox(MeshObjectPtr& skybox, ConstantBufferPtr cb, constant& cc)
{
	cc.m_transform.setIdentity();
	cc.m_transform.setTranslation(skybox->getPosition());
	cc.m_transform.setScale(skybox->getScale());
	cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps_skybox);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps_skybox, cb, 0);


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(skybox->getMesh()->getVertexBuffer());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(skybox->getMesh()->getIndexBuffer());

	for (MaterialIndexRange mir : skybox->getMesh()->getMaterials()) {
		MaterialPtr material = mir.material;

		//SET MATERIAL
		GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, material->getConstantBuffer(), 1);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps_skybox, material->getConstantBuffer(), 1);

		for (auto&& [index, texture] : material->getTextures()) {
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_skybox, texture, index);
		}


		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mir.high - mir.low, 0, mir.low);
	}
}
