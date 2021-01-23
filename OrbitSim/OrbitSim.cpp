#include "OrbitSim.h"
#include <numbers>
#include <sstream>

OrbitSim::OrbitSim(const wchar_t* wWindowName, short sWidth, short sHeight)
	:
	Application(wWindowName, sWidth, sHeight) {
	main = { 3017.58f, Vec2D{0.0f, 0.0f}, Vec2D{0.0f, 0.0f}, 50.0f };
	b2 = { 10.0f, Vec2D{-100.0f, 0.0f}, Vec2D{0.0f, 8.0f}, 10.0f };
	b3 = { 10.0f, Vec2D{-100.0f, 0.0f}, Vec2D{0.0f, 8.0f}, 10.0f };

	// Generate grid, the vector contains the position of the center of each cell relative to the center of the body
	const auto generate_grid = [](const Body<float>& b, const size_t nRes) -> std::vector<Vec2D> {
		std::vector<Vec2D> vGrid;
		vGrid.reserve(nRes * nRes);
		const float fCellWidth = 2.0f * b.get_radius() / static_cast<float>(nRes);
		const float fRadiusSq = b.get_radius() * b.get_radius();

		// The cells will assume to begin in the second quadrant of the body(assumed to be a circle)
		const Vec2D vStart{ -b.get_radius() + fCellWidth / 2.0f, b.get_radius() - fCellWidth / 2.0f };
		for (size_t i = 0; i < nRes; i++) {
			for (size_t j = 0; j < nRes; j++) {
				Vec2D v = vStart + Vec2D{ static_cast<float>(j)* fCellWidth, -1.0f * static_cast<float>(i) * fCellWidth };
				if (Vec2D::magsq(v) < fRadiusSq) {
					vGrid.push_back(v);
				}
			}
		}
		return std::move(vGrid);
	};

	vGrid = generate_grid(main, nRes);
	vGrid.shrink_to_fit();
}

void OrbitSim::run(const float fTime) {
	using namespace math;
	constexpr float t = 0.1f;
	
	// Calculate acceleration on b1 from b2
	Vec2D vPos{};
	const auto calculate_acceleration = [&](const Body<float>& satellite, const Body<float>& body, const std::vector<Vec2D>& vGrid, const size_t nGridRes) -> Vec2D {
		constexpr float G = 2.0f;
		const float fCellWidth = 2.0f * body.get_radius() / static_cast<float>(nGridRes);
		const float fDensity = body.get_mass() / (static_cast<float>(std::numbers::pi) * body.get_radius() * body.get_radius());
		Vec2D v2Acceleration{};

		float fMassSum = 0.0f;
		for (const auto& v : vGrid) {
			Vec2D v2SatToB_unit = v - satellite.get_pos();
			float fSatToB_sq = Vector<>::magsq(v2SatToB_unit);
			v2SatToB_unit /= sqrt(fSatToB_sq);
			vPos += v2SatToB_unit;
			fMassSum += fCellWidth * fCellWidth * fDensity;

			v2Acceleration += (G * fDensity * fCellWidth * fCellWidth * v2SatToB_unit / fSatToB_sq);
		}
		std::wostringstream oss;
		oss << L"\nCalculated mass of grid: " << fMassSum << L'\n';
		OutputDebugStringW(oss.str().c_str());
		return std::move(v2Acceleration);
	};
	Vec2D v2Acceleration = calculate_acceleration(b3, main, vGrid, nRes);
	vPos /= sqrt(Vector<>::magsq(vPos));
	b3.set_vel(b3.get_vel() + t * v2Acceleration);
	b3.set_pos(b3.get_pos() + t * b3.get_vel());

	
	// Analytical Result
	Vec2D r2_1 = main.get_pos() - b2.get_pos();
	float rsq = Vec2D::magsq(r2_1);
	r2_1 /= sqrt(rsq);

	constexpr float G = 2.0f;
	Vec2D v2acc = G * main.get_mass() * r2_1 / rsq;

	std::wostringstream oss;
	oss << L"\nNumerical acceleration: " << v2Acceleration << L'\n';
	oss << L"Analytical acceleration: " << v2acc << L'\n';
	OutputDebugStringW(oss.str().c_str());

	b2.set_vel(b2.get_vel() + t * v2acc);
	b2.set_pos(b2.get_pos() + t * b2.get_vel());


	/*ptrGfx->draw_circle(b1);
	ptrGfx->draw_circle(b2);
	ptrGfx->draw_circle(b1);
	ptrGfx->draw_square(b2);*/

	ptrGfx->draw_circle(b2);
	ptrGfx->draw_circle(b3, GFX::Color::Red);
	//ptrGfx->draw_circle(b1);
	ptrGfx->draw_circle_with_grid(main, vGrid, 2.0f * main.get_radius() / static_cast<float>(nRes));
	//b2Tob1 = sqrt(Vec2D::magsq(b1.get_pos() - b2.get_pos())) * v2acc / sqrt(Vec2D::magsq(v2acc));
	//ptrGfx->draw_line(b2Tob1.get_x(), b2Tob1.get_y(), b2.get_pos().get_x(), b2.get_pos().get_y());
	/*for (auto v = grid.begin(); v != (grid.begin() + nCount); v++) {
		const Vec2D vTmp = *v + b1Center;
		ptrGfx->draw_square(vTmp.get_x(), vTmp.get_y(), w);
	}*/
}