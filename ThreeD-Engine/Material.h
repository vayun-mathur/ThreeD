#pragma once
#include "Resource.h"
#include "prerequisites.h"
#include <map>
#include "Vector.h"
namespace tinyobj {
	struct material_t;
}

enum class CULL_MODE {
	FRONT = 0,
	BACK = 1
};

class Material
{
public:
	Material(tinyobj::material_t* objmat);
	~Material();

	inline CULL_MODE getCullMode() { return m_cull_mode; }
	inline void setCullMode(CULL_MODE cull_mode) { m_cull_mode = cull_mode; }
	inline std::map<unsigned int, TexturePtr>& getTextures() { return m_tex; }
	inline ConstantBufferPtr getConstantBuffer() { return m_cb; }
	inline vec3 getKA() { return ka; }
	inline vec3 getKD() { return kd; }
	inline vec3 getKS() { return ks; }
private:
	vec3 ka, kd, ks;
	float shininess;
private:
	TexturePtr map_ka, map_kd, map_ks;
	std::map<unsigned int, TexturePtr> m_tex;
	ConstantBufferPtr m_cb;
	CULL_MODE m_cull_mode = CULL_MODE::BACK;
};

