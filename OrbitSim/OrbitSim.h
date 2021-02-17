#include "Application.h"
#include "MathVector.h"
#include <numbers>
#include <random>

#include "Graph.h"

#ifndef ORBITSIM_H
#define ORBITSIM_H

template<typename T = float>
class Body {
private:
	float fMass;

	clm::math::Vector<T> v2Pos;
	clm::math::Vector<T> v2Vel;

	float fRadius;

public:
	bool bStationary;

	Body(float fMass_in = 0.0f, clm::math::Vector<T> v2Pos_in = {}, clm::math::Vector<T> v2Vel_in = {}, float fRadius_in = 0.0f, bool bStationary_in = false) {
		fMass = fMass_in;
		v2Pos = v2Pos_in;
		v2Vel = v2Vel_in;
		fRadius = fRadius_in;
		bStationary = bStationary_in;
	}

	~Body() {
		fMass = 0.0f;
		v2Pos = {};
		v2Vel = {};
		fRadius = 0.0f;
		bStationary = false;
	}
	Body(const Body&) = default;
	Body(Body&&) = default;
	Body& operator=(Body&&) noexcept = default;
	Body& operator=(const Body&) = default;

	const clm::math::Vector<T>& get_pos() const { return v2Pos; }
	const clm::math::Vector<T>& get_vel() const { return v2Vel; }
	T get_radius() const { return fRadius; }
	T get_mass() const { return fMass; }

	void set_pos(clm::math::Vector<T> v2Pos_in) {
		v2Pos = v2Pos_in;
	}
	void set_vel(clm::math::Vector<T> v2Vel_in) {
		v2Vel = v2Vel_in;
	}
	T sample_density(T r, T t) {
		return fMass / (pow(fRadius, 2) * static_cast<T>(std::numbers::pi));
	}
	std::vector<T> sample_density(const std::vector<std::pair<T, T>>& pts) const {
		std::vector<T> vRet;
		vRet.reserve(pts.size());
		
		const T fDensity = fMass / (powf(fRadius, 2) * static_cast<T>(std::numbers::pi));
		for (size_t i = 0; i < pts.size(); i++) {
			vRet.push_back(fDensity);
		}
		return std::move(vRet);
	}
	operator clm::Geometry::Circle() {
		return { v2Pos.get_x(), v2Pos.get_y(), fRadius };
	}
};

class Region {
protected:
	struct Dimensions {
		clm::math::Point2D_F top_left;
		clm::math::Point2D_F bot_right;
	} dim;

public:
	Region() : dim({}) {}
	Region(float left, float top, float right, float bottom) : dim({ { left, top }, { right, bottom } }) {}
	Region(clm::math::Point2D_F top_left_in, clm::math::Point2D_F bot_right_in) : dim({ top_left_in, bot_right_in }) {}
	virtual void update_dimensions(float, float, float, float);
	inline clm::math::Point2D_F to_gfx_coordinates(const clm::math::Point2D_F&) const noexcept;
	inline clm::Geometry::Rect to_gfx_coordinates(const clm::Geometry::Rect&) const noexcept;
	virtual void draw(const clm::Graphics&) const = 0;
};

class OrbitSimRegion : public Region {
public:
	/*Body<float> main;
	Body<float> b2;
	Body<float> b3;*/
	std::vector<Body<float>> vElements;

	const size_t nRes = 10u;
	std::vector<clm::math::Vec2D_F> vGrid;
//public:
	OrbitSimRegion() : Region() {}
	OrbitSimRegion(float left, float top, float right, float bottom) : Region(left, top, right, bottom) {}
	OrbitSimRegion(clm::math::Point2D_F top_left_in, clm::math::Point2D_F bot_right_in) : Region(top_left_in, bot_right_in) {}

	void add_body(const Body<float>& b) { vElements.emplace_back(b); }

	void draw(const clm::Graphics&) const override;
};

class GraphRegion : public Region {
private:
	clm::Graph gph;
public:
	GraphRegion() : Region(), gph({}) {}
	GraphRegion(float left, float top, float right, float bottom) : Region(left, top, right, bottom), gph({left, top, right, bottom}) {}

	virtual void update_dimensions(float, float, float, float) override;
	void draw(const clm::Graphics&) const override;
};

class OrbitSim : public clm::Application {
private:
	/*Body<float> main;
	Body<float> b2;
	Body<float> b3;*/

	OrbitSimRegion osr;
	GraphRegion gr;

	const size_t nRes = 10u;
	std::vector<clm::math::Vec2D_F> vGrid;

	template<typename T = float>
	class MonteCarloRNG {
	public:
		MonteCarloRNG() : rng({}) { rng.seed(0); }
		~MonteCarloRNG() = default;
		T operator()() { return static_cast<T>(rng()) / static_cast<T>(std::numeric_limits<unsigned int>::max()); }
	private:
		std::mt19937 rng;
	};
	MonteCarloRNG<> rng;
public:
	OrbitSim(const wchar_t*, short, short);
	OrbitSim(const Application&) = delete;
	OrbitSim& operator=(const Application) = delete;

	void run(const float) override;
};

#endif