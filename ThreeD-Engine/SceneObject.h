#pragma once
#include <string>
#include <map>
#include "prerequisites.h"
class SceneSystem;
class SceneObject
{
public:
	SceneObject(std::string name);

	void addChild(SceneObjectPtr child);

	template<typename type>
	inline std::shared_ptr<type> getChild(std::string name) const {
		return std::reinterpret_pointer_cast<type>(m_children.at(name));
	}

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
	friend class SceneSystem;
};