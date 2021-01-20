#include "Application.h"

#ifndef ORBITSIM_H
#define ORBITSIM_H

template<typename T = float>
class Vec2D {
private:
	T x, y;
public:
	Vec2D(T tX_in = 0.0f, T tY_in = 0.0f) { x = tX_in; y = tY_in; }
	~Vec2D() { x = 0.0f; y = 0.0f; }
	Vec2D operator=(const Vec2D& v) { this->x = v.x; this->y = v.y; return *this; }

	T get_x() { return x; }
	T get_y() { return y; }

	const Vec2D<T> operator+(const Vec2D<T>& rhs) const {
		return { this->x + rhs.x, this->y + rhs.y };
	}
	const Vec2D<T> operator-(const Vec2D<T>& rhs) const {
		return { this->x - rhs.x, this->y - rhs.y };
	}
	
	const Vec2D<T>& operator/=(T f) {
		this->x /= f;
		this->y /= f;
		return *this;
	}

	const Vec2D<T> operator*(T f) const {
		return { this->x * f, this->y * f };
	}
	friend const Vec2D<T> operator*(T f, Vec2D<T> v) {
		return { v.x * f, v.y * f };
	}
	const Vec2D<T> operator/(T f) const {
		return { this->x / f, this->y / f };
	}

	static T dot(Vec2D<T> v1, Vec2D<T> v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
};

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
	Body& operator=(Body&&) = default;
	Body& operator=(Body&) = default;

	const Vec2D<T>& get_pos() { return v2Pos; }
	const Vec2D<T>& get_vel() { return v2Vel; }
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