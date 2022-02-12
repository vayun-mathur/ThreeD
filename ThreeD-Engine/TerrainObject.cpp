#include "TerrainObject.h"
#include "AppWindow.h"
#include "Material.h"
#include "VertexMesh.h"
#include <tiny_obj_loader.h>

const int _SIZE = 20;
const int VERTEX_COUNT = 800;

TerrainObject::TerrainObject(std::string name, SceneSystem* system, VertexShaderPtr vs, PixelShaderPtr ps)
	: SceneObject(name, system), m_vs(vs), m_ps(ps)
{
	//TODO: Complete this
	std::vector<VertexMesh> list_vertices;
	std::vector<unsigned int> list_indices;
	std::vector<MaterialIndexRange> materials;

	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			list_vertices.push_back(VertexMesh(vec3(j / (VERTEX_COUNT - 1) * _SIZE, 0, i / (VERTEX_COUNT - 1) * _SIZE), vec2(j / (VERTEX_COUNT - 1), i / (VERTEX_COUNT - 1)), vec3(0, 1, 0)));
		}
	}
	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			list_indices.push_back(i * VERTEX_COUNT + j);
			list_indices.push_back((i + 1) * VERTEX_COUNT + j);
			list_indices.push_back(i * VERTEX_COUNT + j + 1);
			list_indices.push_back(i * VERTEX_COUNT + j + 1);
			list_indices.push_back((i + 1) * VERTEX_COUNT + j);
			list_indices.push_back((i + 1) * VERTEX_COUNT + j + 1);
		}
	}

	tinyobj::material_t* mat = new tinyobj::material_t();
	mat->ambient[0] = 0.5;
	mat->ambient[1] = 0.5;
	mat->ambient[2] = 0.5;
	materials.push_back({ 0, (int)list_indices.size(), std::make_shared<Material>(mat) });

	m_mesh = std::make_shared<Mesh>(list_vertices, list_indices, materials);
}

TerrainObject::~TerrainObject()
{
}

ScriptValue* TerrainObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (s == "position") return new Vec3ScriptValue(&m_position);
	if (s == "scale") return new Vec3ScriptValue(&m_scale);
	return nullptr;
}

void TerrainObject::render(ConstantBufferPtr cb)
{
	AppWindow::s_main->setConstantBuffer(*this);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//SET CONSTANT BUFFER
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, cb, 0);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, cb, 0);

	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
	//SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());

	for (MaterialIndexRange mir : m_mesh->getMaterials()) {
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
