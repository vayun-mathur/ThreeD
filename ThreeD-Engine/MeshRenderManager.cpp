#include "MeshRenderManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "PhysicalObject.h"
#include "AppWindow.h"
#include "MeshObject.h"
#include "EditableMesh.h"

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

void MeshRenderManager::render(std::vector<PhysicalObjectPtr>& meshes, ConstantBufferPtr cb, constant& cc)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

	cc.m_transform.setIdentity();
	cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);

	for (PhysicalObjectPtr mesh : meshes) {


		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getMesh()->getVertexBuffer());

		for (MIR mir : mesh->getMesh()->getMaterialIterable()) {
			MaterialPtr material = mir.material;

			GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, material->getConstantBuffer(), 1);
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, material->getConstantBuffer(), 1);

			for (auto&& [index, texture] : material->getTextures()) {
				GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, texture, index);
			}

			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleList(mir.high - mir.low, mir.low);
		}
	}
}
