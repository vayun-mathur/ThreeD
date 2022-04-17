#pragma once
#include "prerequisites.h"
#include "SceneObject.h"
#include <vector>

class SceneSystem
{
public:
	SceneSystem(std::wstring filepath);
	~SceneSystem();
public:
	void init();
	void update(double delta_time);

	inline CameraObjectPtr getCamera() { return m_main_camera; }
	inline SceneObjectPtr getRoot() { return m_root; }
private:
	SceneObjectPtr m_root;
	CameraObjectPtr m_main_camera;
};

