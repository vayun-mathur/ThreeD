#include "App/AppWindow.h"
#include "Engine/InputSystem/InputSystem.h"

int main() {
	try {
		GraphicsEngine::create();
		InputSystem::create();
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