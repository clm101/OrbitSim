#include "OrbitSim.h"
#include <numbers>

OrbitSim::OrbitSim(const wchar_t* wWindowName, short sWidth, short sHeight)
	:
	Application(wWindowName, sWidth, sHeight) {
	b1 = { 3000.0f, {640.0f, 360.0f}, {0.0f, 0.0f}, 50.0f };
	b2 = { 10.0f, {520.0f, 360.0f}, {0.0f, 6.0f}, 10.0f };
}

void OrbitSim::run(const float fTime) {
	constexpr float G = 2.0f;
	constexpr float t = 0.2f;
	constexpr size_t nRes = 4u;
	
	/*const float w = 2 * b1.get_radius() / nRes;
	const Vec2D<float>& b1Center = b1.get_pos();
	const float b1_x = b1Center.get_x();
	const float b1_y = b1Center.get_y();
	const float rsq = b1.get_radius() * b1.get_radius();
	const float fDensity = b1.get_mass() / (std::numbers::pi * rsq);
	
	std::vector<Vec2D<float>> grid(static_cast<size_t>(nRes) * static_cast<size_t>(nRes));
	auto gridIter = grid.begin();
	size_t nCount = 0;
	Vec2D<float> vStart = { w - b1.get_radius(), w - b1.get_radius() };
	for (size_t i = 0; i < nRes; i++) {
		for (size_t j = 0; j < nRes; j++) {
			const Vec2D<float> v = vStart + Vec2D<float>{ i * w, j * w};
			if (Vec2D<float>::magsq(v) < rsq) {
				*gridIter = v;
				gridIter++;
				nCount++;
			}
		}
	}

	Vec2D<float> v2acc{};
	Vec2D<float> b2Tob1 = b1.get_pos() - b2.get_pos();
	for (auto v = grid.begin(); v != (grid.begin() + nCount); v++) {
		const Vec2D<float> rUnit = (*v + b2Tob1) / sqrt(Vec2D<float>::magsq(*v + b2Tob1));
		v2acc += (G * fDensity * w * w * rUnit / Vec2D<float>::magsq(*v + b2Tob1));
	}*/

	Vec2D r2_1 = b1.get_pos() - b2.get_pos();
	float rsq = Vec2D<float>::magsq(r2_1);
	r2_1 /= sqrt(rsq);

	Vec2D v2acc = G * b1.get_mass() * r2_1 / rsq;

	b2.set_vel(b2.get_vel() + t * v2acc);
	b2.set_pos(b2.get_pos() + t * b2.get_vel());


	/*ptrGfx->draw_circle(b1);
	ptrGfx->draw_circle(b2);
	ptrGfx->draw_circle(b1);
	ptrGfx->draw_square(b2);*/

	ptrGfx->draw_circle(b2);
	//ptrGfx->draw_circle(b1);
	ptrGfx->draw_circle_with_grid(b1, nRes);
	//b2Tob1 = sqrt(Vec2D<float>::magsq(b1.get_pos() - b2.get_pos())) * v2acc / sqrt(Vec2D<float>::magsq(v2acc));
	//ptrGfx->draw_line(b2Tob1.get_x(), b2Tob1.get_y(), b2.get_pos().get_x(), b2.get_pos().get_y());
	/*for (auto v = grid.begin(); v != (grid.begin() + nCount); v++) {
		const Vec2D<float> vTmp = *v + b1Center;
		ptrGfx->draw_square(vTmp.get_x(), vTmp.get_y(), w);
	}*/
}