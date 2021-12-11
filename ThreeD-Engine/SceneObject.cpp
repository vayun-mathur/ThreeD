#include "SceneObject.h"
#include "SceneSystem.h"

SceneObject::SceneObject(std::string name, SceneSystem* system)
	:m_name(name), m_system(system)
{
}

void SceneObject::addChild(SceneObjectPtr child)
{
	m_children.insert({ child->m_name, child });
	child->m_parent = std::shared_ptr<SceneObject>(this);
}

SceneObjectPtr SceneObject::getChild(std::string name) const
{
	return m_children.at(name);
}

void SceneObject::removeChild(std::string name)
{
	m_children.erase(name);
}

void SceneObject::init()
{
	for (auto&& [name, child] : m_children) {
		child->init();
	}
}

void SceneObject::update(double delta_time)
{
	for (auto&& [name, child] : m_children) {
		child->update(delta_time);
	}
}

void SceneObject::render(ConstantBufferPtr cb)
{
	for (auto&& [name, child] : m_children) {
		child->render(cb);
	}
}

SceneObject::~SceneObject()
{
}

ScriptValue* SceneObject::dot(std::string s)
{
	if (s == "parent") return m_parent.get();
	if (m_children.find(s) != m_children.end()) {
		return m_children[s].get();
	}
	return nullptr;
}
