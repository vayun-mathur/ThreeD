#include "MeshObject.h"
#include "AppWindow.h"

MeshObject::MeshObject(std::string name, std::wstring mesh_location, std::wstring tex_location, VertexShaderPtr vs, PixelShaderPtr ps,
	bool front_cull)
	: SceneObject(name), m_front_cull(front_cull)
{
	m_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(tex_location.c_str());
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(mesh_location.c_str());

	m_vs = vs;
	m_ps = ps;
}

MeshObject::~MeshObject()
{
}

void MeshObject::render(ConstantBufferPtr cb)
{
	AppWindow::s_main->setConstantBuffer(*this);
	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(m_front_cull);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_tex);

	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
	//SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());


	// FINALLY DRAW THE TRIANGLE
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(m_mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
}
