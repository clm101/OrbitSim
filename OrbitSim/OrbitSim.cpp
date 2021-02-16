#include "OrbitSim.h"
#include <numbers>
#include <sstream>
#include <chrono>

void Region::update_dimensions(float left, float top, float right, float bottom) {
	dim.top_left = { left, top };
	dim.bot_right = { right, bottom };
}

void OrbitSimRegion::draw(const Graphics& gfx) const {
	for (const auto& i : vElements) {
		gfx.draw_circle({ i.get_pos().get_x() + 0.5f * (dim.top_left.get_x() + dim.bot_right.get_x()),
						  -1.0f * i.get_pos().get_y() + 0.5f * (dim.top_left.get_y() + dim.bot_right.get_y()),
						  i.get_radius() });
	}
	gfx.draw_line(dim.bot_right.get_x(), dim.top_left.get_y(), dim.bot_right.get_x(), dim.bot_right.get_y());
}

void GraphRegion::draw(const Graphics& gfx) const {
	constexpr float y_scale = 0.1f;
	for (auto i = vDataPoints.begin() + 1; i != vDataPoints.end(); i++) {
		const float x1 = (i - 1)->first + dim.top_left.get_x();
		const float y1 = -1.0f * y_scale * (i - 1)->second + dim.bot_right.get_y();
		const float x2 = i->first + dim.top_left.get_x();
		const float y2 = -1 * y_scale * i->second + dim.bot_right.get_y();
		gfx.draw_line(x1, y1, x2, y2);
	}
}

void GraphRegion::add_energy_calc(float E, float fTime) {
	vDataPoints.emplace_back(E, fTime);
}

OrbitSim::OrbitSim(const wchar_t* wWindowName, short sWidth, short sHeight)
	:
	Application(wWindowName, sWidth, sHeight),
	osr({}),
	gr({})
{
	osr.add_body({ 2500.0f, Vec2D_F{0.0f, 0.0f}, Vec2D_F{0.0f, 0.0f}, 50.0f });
	osr.add_body({ 10.0f, Vec2D_F{-100.0f, 0.0f}, Vec2D_F{0.0f, 8.0f}, 10.0f });
	//osr.add_body({ 10.0f, Vec2D_F{-100.0f, 0.0f}, Vec2D_F{0.0f, 8.0f}, 10.0f });

	Vec2D_F ScreenDim = ptrGfx->get_screen_size();
	osr.update_dimensions(0.0f, 0.0f, ScreenDim.get_x() - 600.0f, ScreenDim.get_y());
	gr.update_dimensions(ScreenDim.get_x() - 600.0f, 0.0f, ScreenDim.get_x(), ScreenDim.get_y());

	// Generate grid, the vector contains the position of the center of each cell relative to the center of the body
	const auto generate_grid = [](const Body<float>& b, const size_t nRes) -> std::vector<Vec2D_F> {
		std::vector<Vec2D_F> vGrid;
		vGrid.reserve(nRes * nRes);
		const float fCellWidth = 2.0f * b.get_radius() / static_cast<float>(nRes);
		const float fRadiusSq = b.get_radius() * b.get_radius();

		// The cells will assume to begin in the second quadrant of the body(assumed to be a circle)
		const Vec2D_F vStart{ -b.get_radius() + fCellWidth / 2.0f, b.get_radius() - fCellWidth / 2.0f };
		for (size_t i = 0; i < nRes; i++) {
			for (size_t j = 0; j < nRes; j++) {
				Vec2D_F v = vStart + Vec2D_F{ static_cast<float>(j)* fCellWidth, -1.0f * static_cast<float>(i) * fCellWidth };
				if (Vec2D_F::magsq(v) < fRadiusSq) {
					vGrid.push_back(v);
				}
			}
		}
		return std::move(vGrid);
	};

	vGrid = generate_grid(osr.vElements[0], nRes);
	vGrid.shrink_to_fit();
}

void OrbitSim::run(const float fTime) {
	using namespace math;
	constexpr float t = 0.1f;

	// Newton's Law of Gravity
	auto newt_acceleration = [](float m, const Vec2D_F& r_Satellite, const Vec2D_F& r_Body) -> Vec2D_F {
		constexpr float G = 2.0f;
		Vec2D_F vR1ToR2_unit = Vector<>::get_unit(r_Satellite, r_Body);
		const float vR1ToR2_mag = Vector<>::magsq(r_Satellite, r_Body);

		return G * m * vR1ToR2_unit / vR1ToR2_mag;
	};

	// Heun's Method to get velocity
	std::array<Vec2D_F, 2> accelerationCalcs{};
	std::array<Vec2D_F, 2> velocityCalcs{};

	accelerationCalcs[0] = newt_acceleration(osr.vElements[0].get_mass(), osr.vElements[1].get_pos(), osr.vElements[0].get_pos());
	velocityCalcs[0] = osr.vElements[1].get_vel() + t * accelerationCalcs[0];
	Vec2D_F predictedPosition = osr.vElements[1].get_pos() + t * velocityCalcs[0];

	accelerationCalcs[1] = newt_acceleration(osr.vElements[0].get_mass(), osr.vElements[1].get_pos(), osr.vElements[0].get_pos());
	velocityCalcs[1] = osr.vElements[1].get_vel() + 0.5f * t * (accelerationCalcs[0] + accelerationCalcs[1]);
	osr.vElements[1].set_vel(velocityCalcs[1]);
	osr.vElements[1].set_pos(osr.vElements[1].get_pos() + 0.5f * t * (velocityCalcs[0] + velocityCalcs[1]));

	
	// Calculate acceleration on b1 from b2
	//Vec2D_F vPos{};
	const auto calculate_acceleration = [&](const Body<float>& satellite, const Body<float>& body, const std::vector<Vec2D_F>& vGrid, const size_t nGridRes) -> Vec2D_F {
		constexpr float G = 2.0f;
		const float fCellWidth = 2.0f * body.get_radius() / static_cast<float>(nGridRes);
		const float fDensity = body.get_mass() / (static_cast<float>(std::numbers::pi) * body.get_radius() * body.get_radius());
		Vec2D_F v2Acceleration{};

		float fMassSum = 0.0f;
		for (const auto& v : vGrid) {
			Vec2D_F v2SatToB_unit = v - satellite.get_pos();
			float fSatToB_sq = Vector<>::magsq(v2SatToB_unit);
			v2SatToB_unit /= sqrt(fSatToB_sq);
			//vPos += v2SatToB_unit;
			fMassSum += fCellWidth * fCellWidth * fDensity;

			//v2Acceleration += (G * fDensity * fCellWidth * fCellWidth * v2SatToB_unit / fSatToB_sq);
			v2Acceleration += newt_acceleration(fDensity * fCellWidth * fCellWidth, satellite.get_pos(), v);
		}
		std::wostringstream oss;
		oss << L"\nCalculated mass of grid: " << fMassSum << L'\n';
		OutputDebugStringW(oss.str().c_str());
		return std::move(v2Acceleration);
	};
	//Vec2D_F v2Acceleration = calculate_acceleration(osr.vElements[2], osr.vElements[0], vGrid, nRes);
	//vPos /= sqrt(Vector<>::magsq(vPos));
	//osr.vElements[2].set_vel(osr.vElements[2].get_vel() + t * v2Acceleration);
	//osr.vElements[2].set_pos(osr.vElements[2].get_pos() + t * osr.vElements[2].get_vel());

	
	// Analytical Result
	/*Vec2D_F r2_1 = osr.vElements[0].get_pos() - osr.vElements[1].get_pos();
	float rsq = Vec2D_F::magsq(r2_1);
	r2_1 /= sqrt(rsq);

	constexpr float G = 2.0f;
	Vec2D_F v2acc = G * osr.vElements[0].get_mass() * r2_1 / rsq;*/

	//std::wostringstream oss;
	//oss << L"\nNumerical acceleration: " << v2Acceleration << L'\n';
	//oss << L"Analytical acceleration: " << v2acc << L'\n';
	//OutputDebugStringW(oss.str().c_str());

	//osr.vElements[1].set_vel(osr.vElements[1].get_vel() + t * v2acc);
	//osr.vElements[1].set_pos(osr.vElements[1].get_pos() + t * osr.vElements[1].get_vel());

	// Calculate Energy
	//float E_b2 = osr.vElements[1].get_mass() * (sqrt(math::Vector<>::magsq(v2acc)) * sqrt(math::Vector<>::magsq(osr.vElements[1].get_pos() - osr.vElements[0].get_pos())) +
	//	0.5f * math::Vector<>::magsq(osr.vElements[1].get_vel()));
	//gr.add_energy_calc(fTime, E_b2);


	/*ptrGfx->draw_circle(b1);
	ptrGfx->draw_circle(b2);
	ptrGfx->draw_circle(b1);
	ptrGfx->draw_square(b2);*/

	//ptrGfx->draw_circle(b2);
	//ptrGfx->draw_circle(b3, GFX::Color::Red);
	//ptrGfx->draw_circle(b1);
	//ptrGfx->draw_circle_with_grid(main, vGrid, 2.0f * main.get_radius() / static_cast<float>(nRes));
	osr.draw(*ptrGfx);
	//gr.draw(*ptrGfx);
	//b2Tob1 = sqrt(Vec2D_F::magsq(b1.get_pos() - b2.get_pos())) * v2acc / sqrt(Vec2D_F::magsq(v2acc));
	//ptrGfx->draw_line(b2Tob1.get_x(), b2Tob1.get_y(), b2.get_pos().get_x(), b2.get_pos().get_y());
	/*for (auto v = grid.begin(); v != (grid.begin() + nCount); v++) {
		const Vec2D_F vTmp = *v + b1Center;
		ptrGfx->draw_square(vTmp.get_x(), vTmp.get_y(), w);
	}*/
}