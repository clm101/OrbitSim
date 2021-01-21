#include "Application.h"
#include "VectorMath.h"

#ifndef ORBITSIM_H
#define ORBITSIM_H

template<typename T = float>
class Body {
private:
	float fMass;

	Vec2D<T> v2Pos;
	Vec2D<T> v2Vel;

	float fRadius;
public:
	Body(float fMass_in = 0.0f, Vec2D<T> v2Pos_in = {}, Vec2D<T> v2Vel_in = {}, float fRadius_in = 0.0f) {
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
	Body(Body&) = default;
	Body(Body&&) = default;
	Body& operator=(Body&&) noexcept = default;
	Body& operator=(Body&) = default;

	const Vec2D<T>& get_pos() const { return v2Pos; }
	const Vec2D<T>& get_vel() const { return v2Vel; }
	float get_radius() const { return fRadius; }

	T get_mass() {
		return fMass;
	}

	void set_pos(Vec2D<T> v2Pos_in) {
		v2Pos = v2Pos_in;
	}
	void set_vel(Vec2D<T> v2Vel_in) {
		v2Vel = v2Vel_in;
	}
	
	operator GFX::Circle() {
		return { v2Pos.get_x(), v2Pos.get_y(), fRadius };
	}
};

class OrbitSim : public Application {
private:
	Body<float> b1;
	Body<float> b2;
public:
	OrbitSim(const wchar_t*, short, short);
	OrbitSim(const Application&) = delete;
	OrbitSim& operator=(const Application) = delete;

	void run(const float) override;
};

#endif