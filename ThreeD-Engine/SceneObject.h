#pragma once
#include <string>
#include <map>
#include "prerequisites.h"
class AppWindow;
class SceneObject
{
public:
	SceneObject(std::string name);

	void addChild(SceneObjectPtr child);
	SceneObjectPtr getChild(std::string name) const;
	void removeChild(std::string name);

	~SceneObject();
private:
	virtual void update(double delta_time);
	virtual void render(ConstantBufferPtr cb);

private:
	std::string m_name;
	std::map<std::string, SceneObjectPtr> m_children;
private:
	friend class AppWindow;
};

