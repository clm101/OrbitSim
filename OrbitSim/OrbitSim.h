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
	Body(float fMass_in = 0.0f, math::Vector<T> v2Pos_in = {}, math::Vector<T> v2Vel_in = {}, float fRadius_in = 0.0f) {
		fMass = fMass_in;
		v2Pos = v2Pos_in;
		v2Vel = v2Vel_in;
		fRadius = fRadius_in;
	}

	~Body() {
		fMass = 0.0f;
		v2Pos = {};
		v2Vel = {};
		fRadius = 0.0f;
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

class OrbitSim : public Application {
private:
	Body<float> main;
	Body<float> b2;
	Body<float> b3;

	const size_t nRes = 10u;
	std::vector<Vec2D> vGrid;
public:
	OrbitSim(const wchar_t*, short, short);
	OrbitSim(const Application&) = delete;
	OrbitSim& operator=(const Application) = delete;

	void run(const float) override;
};

#endif