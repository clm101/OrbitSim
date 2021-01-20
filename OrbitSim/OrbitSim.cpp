#include "OrbitSim.h"

OrbitSim::OrbitSim(const wchar_t* wWindowName, short sWidth, short sHeight)
	:
	Application(wWindowName, sWidth, sHeight) {
	c = { 640.0f, 360.0f, 10.0f };
}

void OrbitSim::run(const float fTime) {
	c.x += 0.1f;

	ptrGfx->draw_circle(c);
}