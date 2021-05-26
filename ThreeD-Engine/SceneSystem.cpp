#include "SceneSystem.h"
#include "MeshObject.h"
#include "CameraObject.h"
#include "DirectionalLightObject.h"
#include "GraphicsEngine.h"
#include "InputSystem.h"
#include <unordered_map>

#include <fstream>
#include <filesystem>

void readString(std::ifstream& stream, std::wstring& out) {
	int len;
	stream >> len;
	char* str = new char[len];
	char c;
	stream.read(&c, 1);
	stream.read(str, len);
	std::string s = std::string(str, len);
	out = std::wstring(s.begin(), s.end());
}
void readString(std::ifstream& stream, std::string& out) {
	int len;
	stream >> len;
	char* str = new char[len];
	char c;
	stream.read(&c, 1);
	stream.read(str, len);
	out = std::string(str, len);
}

SceneSystem::SceneSystem(std::wstring file_path)
{
	m_root = std::make_shared<SceneObject>("root");
	std::wstring full_path = std::filesystem::absolute(file_path);
	std::ifstream scene_file(full_path);


	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	VertexShaderPtr vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	PixelShaderPtr ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyboxShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	PixelShaderPtr skybox_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	std::unordered_map<int, SceneObjectPtr> components;
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
		if (type == "OBJECT") {
			SceneObjectPtr obj = std::make_shared<SceneObject>(name);
			components[parent]->addChild(obj);
			components.insert({ id, obj });
		}
		else if (type == "MESH") {
			std::wstring obj;
			std::wstring tex;
			readString(scene_file, obj);
			readString(scene_file, tex);
			MeshObjectPtr mesh = std::make_shared<MeshObject>(name, obj, tex, vs, ps, false);
			components[parent]->addChild(mesh);
			components.insert({ id, mesh });
		}
		else if (type == "SKYBOX") {
			std::wstring obj;
			std::wstring tex;
			readString(scene_file, obj);
			readString(scene_file, tex);
			MeshObjectPtr skybox = std::make_shared<MeshObject>(name, obj, tex, vs, skybox_ps, true);
			components[parent]->addChild(skybox);
			components.insert({ id, skybox });
		}
		else if (type == "CAMERA") {
			CameraObjectPtr camera = std::make_shared<CameraObject>(name);
			components[parent]->addChild(camera);
			components.insert({ id, camera });
		}
		else if (type == "DIRECTIONAL_LIGHT") {
			vec3 color;
			vec3 direction;
			scene_file >> color.x >> color.y >> color.z >> direction.x >> direction.y >> direction.z;
			DirectionalLightObjectPtr light = std::make_shared<DirectionalLightObject>(name, color, direction);
			components[parent]->addChild(light);
			components.insert({ id, light });
		}
	}
	int main_camera_id, main_light_id;
	scene_file >> main_camera_id >> main_light_id;

	m_main_camera = std::reinterpret_pointer_cast<CameraObject>(components[main_camera_id]);
	m_main_light = std::reinterpret_pointer_cast<DirectionalLightObject>(components[main_light_id]);
}

SceneSystem::~SceneSystem()
{
}

void SceneSystem::update(double delta_time)
{
	m_root->update(delta_time);
}

void SceneSystem::render(ConstantBufferPtr cb)
{
	m_root->render(cb);
}
