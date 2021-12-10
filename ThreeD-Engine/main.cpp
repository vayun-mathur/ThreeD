#include "AppWindow.h"
#include "InputSystem.h"
#include "AudioSystem.h"
#include "ScriptSystem.h"

int main() {
	try {
		GraphicsEngine::create();
		InputSystem::create();
		AudioSystem::create();
		ScriptSystem::create();
	}
	catch (...) { return -1; }
	{
		try {
			AppWindow app;
			while (app.isRun());
		}
		catch (...) {
			InputSystem::release();
			GraphicsEngine::release();
			return -1;
		}
	}

	ScriptSystem::release();
	AudioSystem::release();
	InputSystem::release();
	GraphicsEngine::release();

	return 0;
}