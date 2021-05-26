#include "MeshObject.h"
#include "AppWindow.h"
#include "Material.h"

MeshObject::MeshObject(std::string name, std::wstring mesh_location, std::wstring material_location)
	: SceneObject(name)
{
	m_material = GraphicsEngine::get()->getMaterialManager()->createMaterialFromFile(material_location.c_str());
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(mesh_location.c_str());
}

MeshObject::~MeshObject()
{
}

void MeshObject::render(ConstantBufferPtr cb)
{
	AppWindow::s_main->setConstantBuffer(*this);
	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(m_material->getCullMode());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_material->getVertexShader(), cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_material->getPixelShader(), cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_material->getVertexShader(), m_material->getConstantBuffer(), 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_material->getPixelShader(), m_material->getConstantBuffer(), 1);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_material->getVertexShader());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_material->getPixelShader());

	for (int i = 0; i < m_material->getTextures().size(); i++) {
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_material->getPixelShader(), m_material->getTextures()[i], i);
	}

	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
	//SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());


	// FINALLY DRAW THE TRIANGLE
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(m_mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
}
