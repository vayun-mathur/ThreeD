#include "WaterTileObject.h"
#include "AppWindow.h"
#include "Material.h"
#include "VertexMesh.h"
#include "Perlin.h"
#include <tiny_obj_loader.h>

const int TILE_SIZE = 60;

MeshPtr generateMesh() {
	std::vector<WaterMesh> list_vertices = { WaterMesh(vec2(-1, -1)), WaterMesh(vec2(-1, 1)), WaterMesh(vec2(1, -1)),
		WaterMesh(vec2(1, -1)), WaterMesh(vec2(-1, 1)), WaterMesh(vec2(1, 1)) };

	return std::make_shared<Mesh>(list_vertices);
}

WaterTileObject::WaterTileObject(std::string name, SceneSystem* system, VertexShaderPtr vs, PixelShaderPtr ps)
	: SceneObject(name, system), m_vs(vs), m_ps(ps)
{
	if (!m_mesh) {
		m_mesh = generateMesh();
		m_dudv = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/waterDUDV.png");
		m_normal_map = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/matchingNormalMap.png");
	}
}

WaterTileObject::~WaterTileObject()
{
}

ScriptValue* WaterTileObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "position") return new Vec3ScriptValue(&m_position);
	if (s == "scale") return new Vec3ScriptValue(&m_scale);
	return nullptr;
}

void WaterTileObject::render(ConstantBufferPtr cb)
{
	AppWindow::s_main->setConstantBuffer(*this);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//SET CONSTANT BUFFER
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, AppWindow::s_main->m_wcb, 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, AppWindow::s_main->m_wcb, 1);

	//SET TEXTURES
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_vs, AppWindow::s_main->m_reflection->getTexture(), 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, AppWindow::s_main->m_reflection->getTexture(), 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_vs, AppWindow::s_main->m_refraction->getTexture(), 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, AppWindow::s_main->m_refraction->getTexture(), 1);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_vs, m_dudv, 2);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_dudv, 2);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_vs, m_normal_map, 3);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_normal_map, 3);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_vs, AppWindow::s_main->m_refraction->getDepthTexture(), 4);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, AppWindow::s_main->m_refraction->getDepthTexture(), 4);

	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());

	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(CULL_MODE::BACK);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawTriangleList(6, 0);

}
