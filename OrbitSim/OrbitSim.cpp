#include "OrbitSim.h"


OrbitSim::OrbitSim(const wchar_t* wWindowName, short sWidth, short sHeight)
	:
	Application(wWindowName, sWidth, sHeight) {
	b1 = { 1500.0f, {640.0f, 360.0f}, {0.0f, 0.0f}, 50.0f };
	b2 = { 10.0f, {520.0f, 360.0f}, {0.0f, 12.0f}, 10.0f };
}

void OrbitSim::run(const float fTime) {
	constexpr float G = 15.0f;
	constexpr float t = 0.15f;

	Vec2D r2_1 = b1.get_pos() - b2.get_pos();
	float rsq = Vec2D<float>::dot(r2_1, r2_1);
	r2_1 /= sqrt(rsq);

	Vec2D v2acc = G * b1.get_mass() * r2_1 / rsq;

	b2.set_vel(b2.get_vel() + t * v2acc);
	b2.set_pos(b2.get_pos() + t * b2.get_vel());

	/*ptrGfx->draw_circle(b1);
	ptrGfx->draw_circle(b2);
	ptrGfx->draw_circle(b1);
	ptrGfx->draw_square(b2);*/

	ptrGfx->draw_circle(b2);
	ptrGfx->draw_circle_with_grid(b1, 10u);
}