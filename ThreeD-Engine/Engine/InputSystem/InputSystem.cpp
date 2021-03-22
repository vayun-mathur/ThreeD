#include "InputSystem.h"
#include <Windows.h>


InputSystem* InputSystem::m_system;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::update()
{
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (m_first_time) {
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
		m_first_time = false;
	}

	if (current_mouse_pos.x != m_old_mouse_pos.x || current_mouse_pos.y != m_old_mouse_pos.y) {
		for (InputListener* l : m_map_listeners) {
			l->onMouseMove(Point(current_mouse_pos.x, current_mouse_pos.y));
		}
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
	}

	if (::GetKeyboardState(m_keys_state)) {
		for (unsigned int i = 0; i < 256; i++) {
			if (m_keys_state[i] & 0x80) {
				//KEY DOWN

				for (InputListener* l : m_map_listeners) {
					if (i == VK_LBUTTON) {
						if (m_keys_state[i] != m_old_keys_state[i])
							l->onLeftMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
					}
					else if (i == VK_RBUTTON) {
						if (m_keys_state[i] != m_old_keys_state[i])
							l->onRightMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
					}
					l->onKeyDown(i);
				}
			}
			else {
				//KEY UP
				if (m_keys_state[i] != m_old_keys_state[i]) {
					for (InputListener* l : m_map_listeners) {
						if (i == VK_LBUTTON) {
							l->onLeftMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
						}
						else if (i == VK_RBUTTON) {
							l->onRightMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
						}
						l->onKeyUp(i);
					}
				}
			}
		}
		::memcpy(m_old_keys_state, m_keys_state, sizeof(unsigned char) * 256);
	}
}

void InputSystem::addListener(InputListener* listener)
{

	m_map_listeners.insert(listener);
}

void InputSystem::removeListener(InputListener* listener)
{
	for (auto it = m_map_listeners.begin(); it != m_map_listeners.end(); ++it) {
		if (*it == listener) {
			m_map_listeners.erase(it);
			break;
		}
	}
}

void InputSystem::setCursorPosition(const Point& pos)
{
	::SetCursorPos(pos.x, pos.y);
}

void InputSystem::showCursor(bool show)
{
	::ShowCursor(show);
}

InputSystem* InputSystem::get()
{
	return m_system;
}

void InputSystem::create()
{
	if (InputSystem::m_system) throw std::exception("InputSystem already created");
	InputSystem::m_system = new InputSystem();
}

void InputSystem::release()
{
	if (!InputSystem::m_system) return;
	delete InputSystem::m_system;
	InputSystem::m_system = nullptr;
}