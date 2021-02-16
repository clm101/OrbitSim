#include "Application.h"
#include "MathVector.h"

#ifndef ORBITSIM_H
#define ORBITSIM_H

template<typename T = float>
class Body {
private:
	float fMass;

	math::Vector<T> v2Pos;
	math::Vector<T> v2Vel;

	float fRadius;

public:
	bool bStationary;

	Body(float fMass_in = 0.0f, math::Vector<T> v2Pos_in = {}, math::Vector<T> v2Vel_in = {}, float fRadius_in = 0.0f, bool bStationary_in = false) {
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

	const math::Vector<T>& get_pos() const { return v2Pos; }
	const math::Vector<T>& get_vel() const { return v2Vel; }
	T get_radius() const { return fRadius; }
	T get_mass() const { return fMass; }

	void set_pos(math::Vector<T> v2Pos_in) {
		v2Pos = v2Pos_in;
	}
	void set_vel(math::Vector<T> v2Vel_in) {
		v2Vel = v2Vel_in;
	}
	
	operator GFX::Circle() {
		return { v2Pos.get_x(), v2Pos.get_y(), fRadius };
	}
};

class Region {
protected:
	struct Dimensions {
		Point2D_F top_left;
		Point2D_F bot_right;
	} dim;

public:
	Region() : dim({}) {}
	Region(float left, float top, float right, float bottom) : dim({ { left, top }, { right, bottom } }) {}
	Region(Point2D_F top_left_in, Point2D_F bot_right_in) : dim({ top_left_in, bot_right_in }) {}
	void update_dimensions(float, float, float, float);
	virtual void draw(const Graphics&) const = 0;
};

class OrbitSimRegion : public Region {
public:
	/*Body<float> main;
	Body<float> b2;
	Body<float> b3;*/
	std::vector<Body<float>> vElements;

	const size_t nRes = 10u;
	std::vector<Vec2D_F> vGrid;
//public:
	OrbitSimRegion() : Region() {}
	OrbitSimRegion(float left, float top, float right, float bottom) : Region(left, top, right, bottom) {}
	OrbitSimRegion(Point2D_F top_left_in, Point2D_F bot_right_in) : Region(top_left_in, bot_right_in) {}

	void add_body(const Body<float>& b) { vElements.emplace_back(b); }

	void draw(const Graphics&) const override;
};

class GraphRegion : public Region {
public:
	std::vector<std::pair<float, float>> vDataPoints;
	GraphRegion() : Region(), vDataPoints({}) {}
	GraphRegion(float left, float top, float right, float bottom) : Region(left, top, right, bottom), vDataPoints({}) {}

	void draw(const Graphics&) const override;
	void add_energy_calc(float, float);
};

class OrbitSim : public Application {
private:
	/*Body<float> main;
	Body<float> b2;
	Body<float> b3;*/

	OrbitSimRegion osr;
	GraphRegion gr;

	const size_t nRes = 10u;
	std::vector<Vec2D_F> vGrid;
public:
	OrbitSim(const wchar_t*, short, short);
	OrbitSim(const Application&) = delete;
	OrbitSim& operator=(const Application) = delete;

	void run(const float) override;
};

#endif