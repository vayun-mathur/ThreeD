#pragma once
#include "prerequisites.h"
#include "Frame.h"

class GUIScreen {
public:
	GUIScreen(int width, int height, FramePtr main) : main(main) {}
	FramePtr getMain() { return main; }
private:
	int width;
	int height;
	FramePtr main;
};