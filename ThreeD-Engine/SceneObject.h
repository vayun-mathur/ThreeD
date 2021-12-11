#pragma once
#include <string>
#include <map>
#include <type_traits>
#include "prerequisites.h"
#include "ScriptSystem.h"
#include "Script.h"

enum class SceneObjectType {
	SceneObject, CameraObject, MeshObject, DirectionalLightObject,
	PointLightObject, AudioSourceObject, ScriptObject,
	NumberObject, Vec3Object
};

class SceneSystem;
class SceneObject : public ScriptValue
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


	virtual ScriptValue* dot(std::string);
	virtual ScriptValue* add(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* sub(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* mul(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* div(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* addassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* subassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* mulassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* divassign(ScriptValue* o) { return nullptr; }
	virtual ScriptValue* assign(ScriptValue* o) { return nullptr; }

public:
	inline const std::map<std::string, SceneObjectPtr>& getChildren() { return m_children; }
private:
	virtual void init();
	virtual void update(double delta_time);
	virtual void render(ConstantBufferPtr cb);

private:
	std::string m_name;
	std::map<std::string, SceneObjectPtr> m_children;
protected:
	SceneObjectPtr m_parent;
	SceneSystem* m_system;
public:
	Script* m_click = nullptr;
private:
	friend class SceneSystem;
	friend class SceneObjectInputListener;
};