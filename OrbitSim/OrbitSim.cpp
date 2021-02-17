#include <limits>
#include "OrbitSim.h"
#include <numbers>
#include <sstream>
#include <chrono>
#include <random>


void Region::update_dimensions(float left, float top, float right, float bottom) {
	dim.top_left = { left, top };
	dim.bot_right = { right, bottom };
}

inline clm::math::Point2D_F Region::to_gfx_coordinates(const clm::math::Point2D_F& pt) const noexcept {
	return { pt.get_x() + 0.5f * (dim.top_left.get_x() + dim.bot_right.get_x()),
			-1.0f * pt.get_y() + 0.5f * (dim.top_left.get_y() + dim.bot_right.get_y()) };
}

inline clm::Geometry::Rect Region::to_gfx_coordinates(const clm::Geometry::Rect& pt) const noexcept {
	const float fHalfWidth = 0.5f * (dim.top_left.get_x() + dim.bot_right.get_x());
	const float fHalfHeight = 0.5f * (dim.top_left.get_y() + dim.bot_right.get_y());
	return { pt.fLeft + fHalfWidth, -1.0f * pt.fTop + fHalfHeight,
			pt.fRight + fHalfWidth, -1.0f * pt.fBottom + fHalfHeight };
}

void OrbitSimRegion::draw(const clm::Graphics& gfx) const {
	for (const auto& i : vElements) {
		gfx.draw_circle({ to_gfx_coordinates(i.get_pos()), i.get_radius() });
	}
	gfx.draw_line(dim.bot_right.get_x(), dim.top_left.get_y(), dim.bot_right.get_x(), dim.bot_right.get_y());

	const float fLeft = -0.5f * (dim.top_left.get_x() + dim.bot_right.get_x());
	const float fTop = 0.5f * (dim.top_left.get_y() + dim.bot_right.get_y());
	gfx.write_text(L"Hello, this is a test string that is supposed to be very long", to_gfx_coordinates({fLeft, fTop, fLeft + 100.0f, fTop - 20.0f}));
}

void GraphRegion::update_dimensions(float fLeft, float fTop, float fRight, float fBottom) {
	Region::update_dimensions(fLeft, fTop, fRight, fBottom);
	gph.set_boundary({ fLeft, fTop, fRight, fBottom });
}

void GraphRegion::draw(const clm::Graphics& gfx) const {
	gph.draw(gfx);
}

OrbitSim::OrbitSim(const wchar_t* wWindowName, short sWidth, short sHeight)
	:
	Application(wWindowName, sWidth, sHeight),
	osr({}),
	gr({}),
	rng({})
{
	osr.add_body({ 2500.0f, clm::math::Vec2D_F{0.0f, 0.0f}, clm::math::Vec2D_F{0.0f, 0.0f}, 50.0f });
	osr.add_body({ 10.0f, clm::math::Vec2D_F{-100.0f, 0.0f}, clm::math::Vec2D_F{0.0f, 8.0f}, 10.0f });
	//osr.add_body({ 10.0f, Vec2D_F{-100.0f, 0.0f}, Vec2D_F{0.0f, 8.0f}, 10.0f });

	clm::math::Vec2D_F ScreenDim = ptrGfx->get_screen_size();
	osr.update_dimensions(0.0f, 0.0f, ScreenDim.get_x() - 600.0f, ScreenDim.get_y());
	gr.update_dimensions(ScreenDim.get_x() - 600.0f, 0.0f, ScreenDim.get_x(), ScreenDim.get_y());

	// Generate grid, the vector contains the position of the center of each cell relative to the center of the body
	const auto generate_grid = [](const Body<float>& b, const size_t nRes) -> std::vector<clm::math::Vec2D_F> {
		std::vector<clm::math::Vec2D_F> vGrid;
		vGrid.reserve(nRes * nRes);
		const float fCellWidth = 2.0f * b.get_radius() / static_cast<float>(nRes);
		const float fRadiusSq = b.get_radius() * b.get_radius();

		// The cells will assume to begin in the second quadrant of the body(assumed to be a circle)
		const clm::math::Vec2D_F vStart{ -b.get_radius() + fCellWidth / 2.0f, b.get_radius() - fCellWidth / 2.0f };
		for (size_t i = 0; i < nRes; i++) {
			for (size_t j = 0; j < nRes; j++) {
				clm::math::Vec2D_F v = vStart + clm::math::Vec2D_F{ static_cast<float>(j)* fCellWidth, -1.0f * static_cast<float>(i) * fCellWidth };
				if (clm::math::Vec2D_F::magsq(v) < fRadiusSq) {
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
	constexpr float t = 0.1f;

	// Monte Carlo mass integration
	auto integrate_mass = [&rng = rng](const clm::math::Vec2D_F& o, const Body<float>& b) -> clm::math::Vec2D_F {
		constexpr size_t numOfSamplePoints = 100;
		std::vector<std::pair<float, float>> vPts;
		vPts.reserve(numOfSamplePoints);

		for (size_t i = 0; i < vPts.capacity(); i++) {
			
			//*i = { b.get_radius() * rng(), 2.0f * static_cast<float>(std::numbers::pi) * rng() };
			vPts.push_back({ b.get_radius() * rng(), 2.0f * static_cast<float>(std::numbers::pi) * rng() });
			/*i.first = b.get_radius() * rng();
			i.second = 2.0f * static_cast<float>(std::numbers::pi) * rng();*/
		}

		std::vector<float> vDensityVals = b.sample_density(vPts);
		const float fArea = static_cast<float>(std::numbers::pi) * b.get_radius() * b.get_radius();
		std::vector<clm::math::Vec2D_F> vIntegrand;
		vIntegrand.reserve(numOfSamplePoints);
		const clm::math::Vec2D_F r = b.get_pos() - o;
		for (size_t i = 0; i < vIntegrand.capacity(); i++) {
			const clm::math::Vec2D_F rCalc = r + clm::math::Vec2D_F{ vPts[i].first * std::cosf(vPts[i].second), vPts[i].first * std::sinf(vPts[i].second) };
			vIntegrand.push_back(vDensityVals[i] * fArea * r / powf(clm::math::Vector<>::mag(r), 3.0f));
		}

		clm::math::Vec2D_F v2dRet{};
		for (const clm::math::Vec2D_F& v : vIntegrand) {
			v2dRet += v;
		}
		v2dRet /= numOfSamplePoints;

		return v2dRet;
	};

	// Newton's Law of Gravity
	auto newt_acceleration = [&](const Body<float>& r_Origin, const Body<float>& r_Target) -> clm::math::Vec2D_F {
		constexpr float G = 2.0f;
		clm::math::Vec2D_F vR1ToR2_unit = clm::math::Vector<>::get_unit(r_Origin.get_pos(), r_Target.get_pos());
		const float vR1ToR2_mag = clm::math::Vector<>::magsq(r_Origin.get_pos(), r_Target.get_pos());

		//return G * m * vR1ToR2_unit / vR1ToR2_mag;
		return G * integrate_mass(r_Origin.get_pos(), r_Target);
	};

	// Heun's Method to get velocity
	std::array<clm::math::Vec2D_F, 2> accelerationCalcs{};
	std::array<clm::math::Vec2D_F, 2> velocityCalcs{};

	accelerationCalcs[0] = newt_acceleration(osr.vElements[1], osr.vElements[0]);
	velocityCalcs[0] = osr.vElements[1].get_vel() + t * accelerationCalcs[0];
	clm::math::Vec2D_F predictedPosition = osr.vElements[1].get_pos() + t * velocityCalcs[0];

	accelerationCalcs[1] = newt_acceleration(osr.vElements[1], osr.vElements[0]);
	velocityCalcs[1] = osr.vElements[1].get_vel() + 0.5f * t * (accelerationCalcs[0] + accelerationCalcs[1]);
	osr.vElements[1].set_vel(velocityCalcs[1]);
	osr.vElements[1].set_pos(osr.vElements[1].get_pos() + 0.5f * t * (velocityCalcs[0] + velocityCalcs[1]));

	osr.draw(*ptrGfx);
	gr.draw(*ptrGfx);
}