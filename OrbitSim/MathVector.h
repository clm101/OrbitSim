#ifndef MATHVECTOR_H
#define MATHVECTOR_H

#include <array>
#include <sstream>
#include <functional>

namespace math {
	template<typename T>
	struct plus_equals {
		constexpr T operator()(T& t1, const T& t2) {
			return (t1 += t2);
		}
	};
	template<typename T>
	struct minus_equals {
		constexpr T operator()(T& t1, const T& t2) {
			return (t1 -= t2);
		}
	};
	template<typename T>
	struct multiplies_equals {
		constexpr T operator()(T& t1, const T& t2) {
			return (t1 *= t2);
		}
	};
	template<typename T>
	struct divides_equals {
		constexpr T operator()(T& t1, const T& t2) {
			return (t1 /= t2);
		}
	};

	template<typename T = float, size_t dim = 2>
	class Vector {
	private:
		std::array<T, dim> components;

		template<typename U>
		static constexpr Vector<T, dim> operator_impl_const(const Vector<T, dim>& v1, const Vector<T, dim>& v2, U oper) {
			Vector<T, dim> ret{};
			for (size_t i = 0; i < dim; i++) {
				ret.components[i] = oper(v1.components[i], v2.components[i]);
			}
			return ret;
		}

		template<typename U>
		static constexpr Vector<T, dim> operator_impl_const(const Vector<T, dim>& v1, const T& t, U oper) {
			Vector<T, dim> ret{};
			for (size_t i = 0; i < dim; i++) {
				ret.components[i] = oper(v1.components[i], t);
			}
			return ret;
		}

		template<typename U>
		static constexpr Vector<T, dim> operator_impl(Vector<T, dim>& v1, const Vector<T, dim>& v2, U oper) {
			Vector<T, dim> ret{};
			for (size_t i = 0; i < dim; i++) {
				ret.components[i] = oper(v1.components[i], v2.components[i]);
			}
			return ret;
		}

		template<typename U>
		static constexpr Vector<T, dim> operator_impl(Vector<T, dim>& v1, const T& t, U oper) {
			Vector<T, dim> ret{};
			for (size_t i = 0; i < dim; i++) {
				ret.components[i] = oper(v1.components[i], t);
			}
			return ret;
		}

	public:
		//constexpr Vector() { }
		constexpr Vector(const float x = 0.0f, const float y = 0.0f) {
			components[0] = x;
			components[1] = y;
		}
		constexpr Vector<T,dim> operator=(const Vector<T,dim>& v) { this->components[0] = v.components[0]; this->components[1] = v.components[1]; return *this; }

		T get_x() const { return components[0]; }
		T get_y() const { return components[1]; }

		constexpr Vector<T,dim> operator+(const Vector<T, dim>& rhs) const {
			return operator_impl_const(*this, rhs, std::plus<T>{});
		}
		constexpr Vector<T,dim> operator-(const Vector<T, dim>& rhs) const {
			return operator_impl_const(*this, rhs, std::minus<T>{});
		}

		constexpr Vector<T,dim>& operator+=(Vector<T,dim> rhs) {
			operator_impl(*this, rhs, plus_equals<T>{});
			return *this;
		}
		constexpr Vector<T,dim>& operator-=(Vector<T,dim> rhs) {
			operator_impl(*this, rhs, minus_equals<T>{});
			return *this;
		}
		constexpr Vector<T,dim>& operator/=(T t) {
			operator_impl(*this, t, divides_equals<T>{});
			return *this;
		}

		constexpr Vector<T,dim> operator*(T t) const {
			return operator_impl_const(*this, t, std::multiplies<T>{});
		}
		friend constexpr Vector<T,dim> operator*(T t, Vector<T,dim> v) {
			return operator_impl(v, t, std::multiplies<T>{});
		}
		constexpr Vector<T,dim> operator/(T t) const {
			return operator_impl_const(*this, t, std::divides<T>{});
		}

		friend std::wostream& operator<<(std::wostream& oss, Vector<T,dim> v) {
			return (oss << L"<" << v.get_x() << L", " << v.get_y() << L">");
		}

		static constexpr T dot(Vector<T,dim> v1, Vector<T,dim> v2) {
			T ret{};
			for (size_t i = 0; i < dim; i++) {
				ret += (v1.components[i] * v2.components[i]);
			}
			return ret;
		}

		static constexpr T magsq(Vector<T,dim> v) {
			return dot(v, v);
		}
	};
}

typedef math::Vector<> Point2D;
typedef math::Vector<> Vec2D;

#endif