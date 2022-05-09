#include "MeshRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "AppWindow.h"
#include "MeshObject.h"

__declspec(align(16))
struct skybox_constant
{
	mat4 m_TVM;
};

skybox_constant sc;

void MeshRenderManager::init()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"MeshShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"MeshShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"SkyboxShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs_skybox = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyboxShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps_skybox = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	m_sc = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&sc, sizeof(skybox_constant));
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
	mat4 transform;
	transform.setIdentity();
	transform.setTranslation(skybox->getPosition());
	transform.setScale(skybox->getScale());
	sc.m_TVM = cc.m_projection(cc.m_view(transform));
	m_sc->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &sc);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs_skybox);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps_skybox);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs_skybox, m_sc, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps_skybox, m_sc, 0);


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(skybox->getMesh()->getVertexBuffer());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(skybox->getMesh()->getIndexBuffer());

	for (MaterialIndexRange mir : skybox->getMesh()->getMaterials()) {
		MaterialPtr material = mir.material;

		//SET MATERIAL
		GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());

		for (auto&& [index, texture] : material->getTextures()) {
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps_skybox, texture, index);
		}


		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mir.high - mir.low, 0, mir.low);
	}
}
