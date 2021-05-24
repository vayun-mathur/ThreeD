#include "SceneObject.h"

SceneObject::SceneObject(std::string name)
	:m_name(name)
{
}

void SceneObject::addChild(SceneObjectPtr child)
{
	m_children.insert({ child->m_name, child });
}

SceneObjectPtr SceneObject::getChild(std::string name) const
{
	return m_children.at(name);
}

void SceneObject::removeChild(std::string name)
{
	m_children.erase(name);
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
