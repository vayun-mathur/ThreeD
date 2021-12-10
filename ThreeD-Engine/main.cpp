#include "AppWindow.h"
#include "InputSystem.h"
#include "AudioSystem.h"

int main() {
	try {
		GraphicsEngine::create();
		InputSystem::create();
		AudioSystem::create();
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

	InputSystem::release();
	GraphicsEngine::release();

	return 0;
}