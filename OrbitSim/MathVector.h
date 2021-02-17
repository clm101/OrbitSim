#ifndef MATHVECTOR_H
#define MATHVECTOR_H

#include <array>
#include <sstream>
#include <functional>

namespace clm {
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
				for (size_t i = 0; i < dim; i++) {
					oper(v1.components[i], v2.components[i]);
				}
				return { v1 };
			}

			template<typename U>
			static constexpr Vector<T, dim> operator_impl(Vector<T, dim>& v1, const T& t, U oper) {
				for (size_t i = 0; i < dim; i++) {
					oper(v1.components[i], t);
				}
				return { v1 };
			}

		public:
			constexpr Vector() {
				for (T& e : components) {
					e = static_cast<T>(0);
				}
			}
			constexpr Vector(const float x, const float y) {
				components[0] = x;
				components[1] = y;
			}
			constexpr Vector(const float x, const float y, const float z) {
				components[0] = x;
				components[1] = y;
				components[2] = z;
			}
			constexpr Vector(const Vector& v) {
				for (size_t i = 0; i < dim; i++) {
					this->components[i] = v.components[i];
				}
			}
			constexpr Vector(Vector&& v) noexcept {
				for (size_t i = 0; i < dim; i++) {
					this->components[i] = v.components[i];
					v.components[i] = 0;
				}
			}
			~Vector() {
				for (T& e : components) {
					e = static_cast<T>(0);
				}
			}
			constexpr const Vector<T, dim>& operator=(const Vector<T, dim>& v) {
				for (size_t i = 0; i < dim; i++) {
					this->components[i] = v.components[i];
				}
				return *this;
			}
			constexpr const Vector<T, dim>& operator=(Vector<T, dim>&& v) noexcept {
				for (size_t i = 0; i < dim; i++) {
					this->components[i] = v.components[i];
					v.components[i] = 0;
				}
				return *this;
			}

			constexpr T get_x() const { return components[0]; }
			constexpr T get_y() const { return components[1]; }
			constexpr T get_z() const requires (dim == 3) {
				//static_assert(dim == 3, "Attempting to access third component of a two component vector");
				return components[2];
			}

			constexpr Vector<T, dim> operator+(const Vector<T, dim>& rhs) const {
				return operator_impl_const(*this, rhs, std::plus<T>{});
			}
			constexpr Vector<T, dim> operator-(const Vector<T, dim>& rhs) const {
				return operator_impl_const(*this, rhs, std::minus<T>{});
			}

			constexpr Vector<T, dim>& operator+=(Vector<T, dim> rhs) {
				operator_impl(*this, rhs, plus_equals<T>{});
				return *this;
			}
			constexpr Vector<T, dim>& operator-=(Vector<T, dim> rhs) {
				operator_impl(*this, rhs, minus_equals<T>{});
				return *this;
			}
			constexpr Vector<T, dim>& operator/=(T t) {
				operator_impl(*this, t, divides_equals<T>{});
				return *this;
			}

			constexpr Vector<T, dim> operator*(T t) const {
				return operator_impl_const(*this, t, std::multiplies<T>{});
			}
			friend constexpr Vector<T, dim> operator*(T t, const Vector<T, dim>& v) {
				return operator_impl_const(v, t, std::multiplies<T>{});
			}
			constexpr Vector<T, dim> operator/(T t) const {
				return operator_impl_const(*this, t, std::divides<T>{});
			}

			friend std::wostream& operator<<(std::wostream& oss, Vector<T, dim> v) {
				return (oss << L"<" << v.get_x() << L", " << v.get_y() << L">");
			}

			static constexpr T dot(const Vector<T, dim>& v1, const Vector<T, dim>& v2) {
				T ret = 0;
				for (size_t i = 0; i < dim; i++) {
					ret += (v1.components[i] * v2.components[i]);
				}
				return ret;
			}

			static constexpr T magsq(const Vector<T, dim>& v) {
				return dot(v, v);
			}

			static constexpr T mag(const Vector<T, dim>& v) {
				return sqrt(magsq(v));
			}

			static constexpr T magsq(const Vector<T, dim>& v1, const Vector<T, dim>& v2) {
				return magsq(v2 - v1);
			}

			static constexpr T mag(const Vector<T, dim>& v1, const Vector<T, dim>& v2) {
				return sqrt(magsq(v1, v2));
			}

			static constexpr Vector<T, dim> get_unit(const Vector<T, dim>& v1, const Vector<T, dim>& v2) {
				// From v1 to v2
				Vector<T, dim> vRet = v2 - v1;
				const float vRetMag = mag(vRet);
				vRet /= vRetMag;
				return vRet;
			}
		};

		typedef Vector<float, 2> Point2D_F;
		typedef Vector<float, 2> Vec2D_F;
		typedef Vector<double, 2> Point2D_D;
		typedef Vector<double, 2> Vec2D_D;
		typedef Vector<float, 3> Point3D_F;
		typedef Vector<float, 3> Vec3D_F;
		typedef Vector<double, 3> Point3D_D;
		typedef Vector<double, 3> Vec3D_D;
	}
}

#endif