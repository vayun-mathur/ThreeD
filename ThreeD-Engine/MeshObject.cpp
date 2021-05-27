#include "MeshObject.h"
#include "AppWindow.h"
#include "Material.h"

MeshObject::MeshObject(std::string name, std::wstring mesh_location, VertexShaderPtr vs, PixelShaderPtr ps)
	: SceneObject(name), m_vs(vs), m_ps(ps)
{
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(mesh_location.c_str());
}

MeshObject::~MeshObject()
{
}

void MeshObject::render(ConstantBufferPtr cb)
{
	AppWindow::s_main->setConstantBuffer(*this);
	for (MaterialIndexRange mir : m_mesh->getMaterials()) {
		MaterialPtr material = mir.material;
		GraphicsEngine::get()->getRenderSystem()->setRasterizerState(material->getCullMode());
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, material->getConstantBuffer(), 1);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, material->getConstantBuffer(), 1);

		//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

		for (int i = 0; i < material->getTextures().size(); i++) {
			GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, material->getTextures()[i], i);
		}

		//SET THE VERTICES OF THE TRIANGLE TO DRAW
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
		//SET THE INDICES OF THE TRIANGLE TO DRAW
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());


		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mir.high-mir.low, 0, mir.low);
	}
	
}
