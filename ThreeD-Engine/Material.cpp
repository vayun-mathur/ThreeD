#include "Material.h"
#include "GraphicsEngine.h"
#include "ResourceManager.h"
#include <fstream>

#include <tiny_obj_loader.h>

__declspec(align(16))
struct material_buf
{
	vec3 ambient;
	int has_textures;
	vec3 diffuse;
	float shininess;
	vec3 specular;
};

std::wstring toWide(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

#include <iostream>

Material::Material(tinyobj::material_t* objmat)
{
	ka = vec3(objmat->ambient[0], objmat->ambient[1], objmat->ambient[2]);
	kd = vec3(objmat->diffuse[0], objmat->diffuse[1], objmat->diffuse[2]);
	ks = vec3(objmat->specular[0], objmat->specular[1], objmat->specular[2]);

	shininess = objmat->shininess;

	std::string map_ka_str = objmat->ambient_texname;
	std::string map_kd_str = objmat->diffuse_texname;
	std::string map_ks_str = objmat->specular_texname;
	if (map_ka_str != "") map_ka = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(toWide("Assets\\Meshes\\" + map_ka_str).c_str());
	if (map_kd_str != "") map_kd = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(toWide("Assets\\Meshes\\" + map_kd_str).c_str());
	if (map_ks_str != "") map_ks = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(toWide("Assets\\Meshes\\" + map_ks_str).c_str());

	VertexShaderPtr m_vs;
	PixelShaderPtr m_ps;
	material_buf cc;
	cc.ambient = ka;
	cc.diffuse = kd;
	cc.specular = ks;

	cc.shininess = shininess;

	cc.has_textures = 0;

	if (map_ka != nullptr)
		m_tex.insert({ 0, map_ka }), cc.has_textures += 1;
	if (map_kd != nullptr)
		m_tex.insert({ 1, map_kd }), cc.has_textures += 2;
	if (map_ks != nullptr)
		m_tex.insert({ 2, map_ks }), cc.has_textures += 4;

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(material_buf));
}

Material::~Material()
{
}
