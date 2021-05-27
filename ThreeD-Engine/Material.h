#pragma once
#include "Resource.h"
#include "prerequisites.h"
#include <vector>
#include "Vector.h"
namespace tinyobj {
	struct material_t;
}

enum class CULL_MODE {
	FRONT = 0,
	BACK = 1
};

class Material : public Resource
{
public:
	Material(const wchar_t* full_path);
	Material(tinyobj::material_t* objmat);
	~Material();

	inline CULL_MODE getCullMode() { return m_cull_mode; }
	inline void setCullMode(CULL_MODE cull_mode) { m_cull_mode = cull_mode; }
	inline std::vector<TexturePtr>& getTextures() { return m_tex; }
	inline ConstantBufferPtr getConstantBuffer() { return m_cb; }
	inline float getKA() { return ka; }
	inline float getKD() { return kd; }
	inline float getKS() { return ks; }
	inline vec3 getIA() { return ia; }
	inline vec3 getID() { return id; }
	inline vec3 getIS() { return is; }
private:
	float ka, kd, ks;
	vec3 ia, id, is;
private:
	std::vector<TexturePtr> m_tex;
	ConstantBufferPtr m_cb;
	CULL_MODE m_cull_mode = CULL_MODE::BACK;
};

