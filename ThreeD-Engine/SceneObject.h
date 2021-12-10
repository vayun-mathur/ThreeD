#pragma once
#include <string>
#include <map>
#include <type_traits>
#include "prerequisites.h"

enum class SceneObjectType{
	SceneObject, CameraObject, MeshObject, DirectionalLightObject,
	PointLightObject, AudioSourceObject
};

class SceneSystem;
class SceneObject
{
public:
	SceneObject(std::string name, SceneSystem* system);

	void addChild(SceneObjectPtr child);

	template<typename type>
	inline std::shared_ptr<type> getChild(std::string name) const {
		return std::reinterpret_pointer_cast<type>(m_children.at(name));
	}

	virtual SceneObjectType getType() const {
		return SceneObjectType::SceneObject;
	}

	template<typename type>
	inline type* toType() const {
		return (type*)this;
	}

	SceneObjectPtr getChild(std::string name) const;
	void removeChild(std::string name);

	~SceneObject();

public:
	inline const std::map<std::string, SceneObjectPtr>& getChildren() { return m_children; }
private:
	virtual void update(double delta_time);
	virtual void render(ConstantBufferPtr cb);

private:
	std::string m_name;
	std::map<std::string, SceneObjectPtr> m_children;
protected:
	SceneSystem* m_system;
private:
	friend class SceneSystem;
};