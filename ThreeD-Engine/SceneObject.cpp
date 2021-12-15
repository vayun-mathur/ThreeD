#include "SceneObject.h"
#include "SceneSystem.h"
#include "InputSystem.h"

class SceneObjectInputListener : public InputListener {
public:
	SceneObjectInputListener(SceneObject* obj) : obj(obj) {}
public:
	//KEYBOARD callback functions
	virtual void onKeyDown(int key) {}
	virtual void onKeyUp(int key) {}

	//MOUSE callback functions
	virtual void onMouseMove(const Point& mouse_pos) {}

	virtual void onLeftMouseDown(const Point& mouse_pos) {
		std::map<std::string, ScriptValue*> var_in;
		var_in["button"] = new NumberScriptValue(new float(0));
		obj->m_click.call(obj, var_in);
	}
	virtual void onLeftMouseUp(const Point& mouse_pos) {}

	virtual void onRightMouseDown(const Point& mouse_pos) {
		std::map<std::string, ScriptValue*> var_in;
		var_in["button"] = new NumberScriptValue(new float(1));
		obj->m_click.call(obj, var_in);
	}
	virtual void onRightMouseUp(const Point& mouse_pos) {}
private:
	SceneObject* obj;
};

SceneObject::SceneObject(std::string name, SceneSystem* system)
	:m_name(name), m_system(system)
{
	InputSystem::get()->addListener(new SceneObjectInputListener(this));
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
	if (s == "click") return new CodeValue(&m_click);
	if (m_children.find(s) != m_children.end()) {
		return m_children[s].get();
	}
	return nullptr;
}
