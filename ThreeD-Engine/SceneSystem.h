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
	inline DirectionalLightObjectPtr getLight() { return m_main_light; }
	inline SceneObjectPtr getRoot() { return m_root; }
private:
	SceneObjectPtr m_root;
	CameraObjectPtr m_main_camera;
	DirectionalLightObjectPtr m_main_light;
};

