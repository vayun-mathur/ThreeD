#include "Material.h"
#include "GraphicsEngine.h"
#include "ResourceManager.h"
#include <fstream>

__declspec(align(16))
struct material_buf
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

Material::Material(const wchar_t* full_path)
	:Resource(full_path)
{
	std::ifstream input(full_path);
	std::wstring vs, ps;
	readString(input, vs);
	readString(input, ps);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(vs.c_str(), "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(ps.c_str(), "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	int tex_count;
	input >> tex_count;
	for (int i = 0; i < tex_count; i++) {
		std::wstring tex;
		readString(input, tex);
		m_tex.push_back(GraphicsEngine::get()->getTextureManager()->createTextureFromFile(tex.c_str()));
	}

	input >> ka >> ia.x >> ia.y >> ia.z;
	input >> kd >> id.x >> id.y >> id.z;
	input >> ks >> is.x >> is.y >> is.z;

	int cull;
	input >> cull;
	if (cull == 0) {
		m_cull_mode = CULL_MODE::FRONT;
	}
	else {
		m_cull_mode = CULL_MODE::BACK;
	}

	material_buf cc;
	cc.ambient = vec4(ia.x, ia.y, ia.z, ka);
	cc.diffuse = vec4(id.x, id.y, id.z, kd);
	cc.specular = vec4(is.x, is.y, is.z, ks);

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(material_buf));
}

Material::~Material()
{
}
