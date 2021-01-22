#include <sstream>

#ifndef VECTORMATH_H
#define VECTORMATH_H

template<typename T = float>
class Vec2D {
private:
	T x, y;
public:
	Vec2D(T tX_in = 0.0f, T tY_in = 0.0f) { x = tX_in; y = tY_in; }
	~Vec2D() { x = 0.0f; y = 0.0f; }
	Vec2D operator=(const Vec2D& v) { this->x = v.x; this->y = v.y; return *this; }

	T get_x() const { return x; }
	T get_y() const { return y; }

	const Vec2D<T> operator+(const Vec2D<T>& rhs) const {
		return { this->x + rhs.x, this->y + rhs.y };
	}
	const Vec2D<T> operator-(const Vec2D<T>& rhs) const {
		return { this->x - rhs.x, this->y - rhs.y };
	}

	const Vec2D<T>& operator+=(Vec2D<T> rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}
	const Vec2D<T>& operator-=(Vec2D<T> rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
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

	friend std::wostream& operator<<(std::wostream& oss, Vec2D<T> v) {
		return (oss << L"<" << v.get_x() << L", " << v.get_y() << L">");
	}

	static T dot(Vec2D<T> v1, Vec2D<T> v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	static T magsq(Vec2D<T> v) {
		return dot(v, v);
	}
};

#endif