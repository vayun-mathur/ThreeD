#include "SceneSystem.h"
#include "MeshObject.h"
#include "CameraObject.h"
#include "DirectionalLightObject.h"
#include "PointLightObject.h"
#include "TerrainObject.h"
#include "WaterTileObject.h"
#include "AudioSourceObject.h"
#include "NumberObject.h"
#include "Vec3Object.h"
#include "ScriptObject.h"
#include "GraphicsEngine.h"
#include "InputSystem.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include <unordered_map>

#include <fstream>
#include <sstream>
#include <filesystem>

SceneSystem::SceneSystem(std::wstring file_path)
{
	m_root = std::make_shared<SceneObject>("root", this);
	std::wstring full_path = std::filesystem::absolute(file_path);
	std::ifstream scene_file(full_path);

	std::unordered_map<int, SceneObjectPtr> components;
	std::unordered_map<int, std::string> component_type;
	components.insert({ 0, m_root });
	int component_count;
	scene_file >> component_count;
	for (int i = 0; i < component_count; i++) {
		int id, parent;
		scene_file >> id >> parent;
		std::string name;
		std::string type;
		readString(scene_file, name);
		readString(scene_file, type);
		component_type.insert({ id, type });
		if (type == "OBJECT") {
			SceneObjectPtr obj = std::make_shared<SceneObject>(name, this);
			components[parent]->addChild(obj);
			components.insert({ id, obj });
		}
		else if (type == "MESH") {
			std::wstring obj;
			vec3 position, scale;
			readString(scene_file, obj);
			scene_file >> position.x >> position.y >> position.z >> scale.x >> scale.y >> scale.z;
			MeshObjectPtr mesh = std::make_shared<MeshObject>(name, this, obj);
			mesh->setPosition(position);
			mesh->setScale(scale);
			components[parent]->addChild(mesh);
			components.insert({ id, mesh });
		}
		else if (type == "TERRAIN") {
			TerrainObjectPtr terrain = std::make_shared<TerrainObject>(name, this);
			components[parent]->addChild(terrain);
			components.insert({ id, terrain });
			terrain->getMesh()->getMaterials()[0].material->setCullMode(CULL_MODE::BACK);
		}
		else if (type == "WATER") {
			vec3 position, scale;
			scene_file >> position.x >> position.y >> position.z;
			scene_file >> scale.x >> scale.y >> scale.z;
			WaterTileObjectPtr water = std::make_shared<WaterTileObject>(name, this);
			water->setPosition(position);
			water->setScale(scale);
			components[parent]->addChild(water);
			components.insert({ id, water });
		}
		else if (type == "SKYBOX") {
			std::wstring obj;
			std::wstring tex;
			vec3 position;
			readString(scene_file, obj);
			readString(scene_file, tex);
			scene_file >> position.x >> position.y >> position.z;
			MeshObjectPtr mesh = std::make_shared<MeshObject>(name, this, obj);
			mesh->getMesh()->getMaterials()[0].material->setCullMode(CULL_MODE::FRONT);
			mesh->getMesh()->getMaterials()[0].material->getTextures().insert({0,
				GraphicsEngine::get()->getTextureManager()->createTextureFromFile(tex.c_str()) });
			mesh->getMesh()->getMaterials()[0].material->getTextures().insert({ 1,
				GraphicsEngine::get()->getTextureManager()->createTextureFromFile(tex.c_str()) });
			mesh->setPosition(position);
			components[parent]->addChild(mesh);
			components.insert({ id, mesh });
		}
		else if (type == "CAMERA") {
			float clip_dist, speed;
			scene_file >> clip_dist >> speed;
			CameraObjectPtr camera = std::make_shared<CameraObject>(name, this, clip_dist, speed);
			components[parent]->addChild(camera);
			components.insert({ id, camera });
		}
		else if (type == "DIRECTIONAL_LIGHT") {
			vec3 color;
			vec3 direction;
			scene_file >> color.x >> color.y >> color.z >> direction.x >> direction.y >> direction.z;
			DirectionalLightObjectPtr light = std::make_shared<DirectionalLightObject>(name, this, color, direction);
			components[parent]->addChild(light);
			components.insert({ id, light });
		}
		else if (type == "POINT_LIGHT") {
			vec3 color;
			vec3 position;
			vec3 attenuation;
			float radius;
			scene_file >> color.x >> color.y >> color.z >> position.x >> position.y >> position.z;
			scene_file >> radius;
			scene_file >> attenuation.x >> attenuation.y >> attenuation.z;
			PointLightObjectPtr light = std::make_shared<PointLightObject>(name, this, color, position, radius, attenuation);
			components[parent]->addChild(light);
			components.insert({ id, light });
		}
		else if (type == "AUDIO_SOURCE") {
			vec3 position;
			scene_file >> position.x >> position.y >> position.z;
			AudioSourceObjectPtr audiosource = std::make_shared<AudioSourceObject>(name, this, position);
			components[parent]->addChild(audiosource);
			components.insert({ id, audiosource });
		}
		else if (type == "NUMBER_VALUE") {
			float val;
			scene_file >> val;
			NumberObjectPtr num = std::make_shared<NumberObject>(name, this, val);
			components[parent]->addChild(num);
			components.insert({ id, num });
		}
		else if (type == "VECTOR_VALUE") {
			vec3 val;
			scene_file >> val.x >> val.y >> val.z;
			Vec3ObjectPtr vec = std::make_shared<Vec3Object>(name, this, val);
			components[parent]->addChild(vec);
			components.insert({ id, vec });
		}
		else if (type == "SCRIPT") {
			std::wstring file;
			readString(scene_file, file);
			std::ifstream in(file.c_str());
			std::stringstream buffer;
			buffer << in.rdbuf();
			ScriptObjectPtr script = std::make_shared<ScriptObject>(name, this, Script(buffer.str()));
			components[parent]->addChild(script);
			components.insert({ id, script });
		}
	}
	int main_camera_id;
	scene_file >> main_camera_id;

	m_main_camera = std::reinterpret_pointer_cast<CameraObject>(components[main_camera_id]);
}

SceneSystem::~SceneSystem()
{
}
void SceneSystem::init()
{
	m_root->init();
}

void SceneSystem::update(double delta_time)
{
	m_root->update(delta_time);
}
