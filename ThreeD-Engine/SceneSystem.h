#pragma once
#include "prerequisites.h"
#include "SceneObject.h"


class SceneSystem
{
public:
	SceneSystem(std::wstring filepath);
	~SceneSystem();
public:
	void update(double delta_time);
	void render(ConstantBufferPtr cb);

	inline CameraObjectPtr getCamera() { return m_main_camera; }
	inline int getLightType() { return m_main_light.type; }
	inline DirectionalLightObjectPtr getDirectionalLight() { return m_main_light.directional; }
	inline PointLightObjectPtr getPointLight() { return m_main_light.point; }
	inline SceneObjectPtr getRoot() { return m_root; }
private:
	SceneObjectPtr m_root;
	CameraObjectPtr m_main_camera;
	struct {
		DirectionalLightObjectPtr directional;
		PointLightObjectPtr point;
		int type;
	} m_main_light;
};

