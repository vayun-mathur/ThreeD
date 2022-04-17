#include "WaterTileObject.h"
#include "AppWindow.h"
#include "Material.h"
#include "VertexMesh.h"
#include "Perlin.h"
#include <tiny_obj_loader.h>

const int TILE_SIZE = 60;

WaterTileObject::WaterTileObject(std::string name, SceneSystem* system)
	: SceneObject(name, system)
{
	m_dudv = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/waterDUDV.png");
	m_normal_map = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets/Textures/matchingNormalMap.png");
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