#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>

#include <QDataStream>
#include <QMetaType>
#include <QtDebug>
#include <QPoint>
#include <QSize>

#include "smoothvar.h"


#define INITIALISATION \
		x(data_[0]), y(data_[1]), z(data_[2]), w(data_[3]), \
		r(data_[0]), g(data_[1]), b(data_[2]), a(data_[3]), \
		s(data_[0]), t(data_[1]), p(data_[2]), q(data_[3])

template <typename T>
T interpolate(T tl, T tr, T bl, T br,
			  T weightX, T weightY, T cWeightX, T cWeightY) {

	return weightX * weightY * br +
		   cWeightX * weightY * bl +
		   weightX * cWeightY * tr +
		   cWeightX * cWeightY * tl;
}

template <typename T, unsigned int N>
class Vector {
public:
	Vector()
		: INITIALISATION {
		std::fill(data_, data_ + N, 0);
	}

	Vector(const T& a)
		: INITIALISATION {
		data_[0] = a;
	}

	Vector(const T& a, const T& b)
		: INITIALISATION {
		data_[0] = a;
		data_[1] = b;
	}

	Vector(const T& a, const T& b, const T& c)
		: INITIALISATION {
		data_[0] = a;
		data_[1] = b;
		data_[2] = c;
	}

	Vector(const T& a, const T& b, const T& c, const T& d)
		: INITIALISATION {
		data_[0] = a;
		data_[1] = b;
		data_[2] = c;
		data_[3] = d;
	}

	Vector(const T* array) : INITIALISATION {
		std::copy(array, array + N, data_);
	}

	Vector(const Vector<T,N>& other) : INITIALISATION {
		std::copy(other.data_, other.data_ + N, data_);
	}

	Vector(const SmoothVar<T>* other) : INITIALISATION {
		operator =(other);
	}
	
	Vector(const QPoint& other) : INITIALISATION {
		data_[0] = other.x();
		data_[1] = other.y();
	}
	
	Vector(const QSize& other) : INITIALISATION {
		data_[0] = other.width();
		data_[1] = other.height();
	}

	Vector<T,N>& operator =(const SmoothVar<T>* other) {
		for (unsigned int i = 0; i < N; ++i)
			data_[i] = other[i].currentValue();

		return *this;
	}

	const T& operator [](unsigned int i) const {
		return data_[i];
	}

	T& operator [](unsigned int i) {
		return data_[i];
	}

	T* data() {
		return data_;
	}

	const T* data() const {
		return data_;
	}

	void normalise() {
		std::transform(data_, data_ + N, data_, std::bind2nd(std::divides<T>(), len()));
	}

	T len() {
		return std::sqrt(std::accumulate(data_, data_ + N, 0, square_and_add()));
	}

	static Vector<T,N> interpolate(const Vector<T,N>& tl, const Vector<T,N>& tr,
							const Vector<T,N>& bl, const Vector<T,N>& br,
							T weightX, T weightY);

	T distanceTo(const Vector<T,N>& other) const {
		return (other - *this).len();
	}
	
	T angle() const {
		return std::atan2(y, x);
	}

	Vector<T,N>& operator =(const Vector<T,N>& other) {
		std::copy(other.data_, other.data_ + N, data_);
		return *this;
	}

	Vector<T,N>& operator =(const T* array) {
		std::copy(array, array + N, data_);
	}

	bool operator ==(const Vector<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N, other.data_, other.data_ + N, std::equal_to<T>());
	}

	bool operator !=(const Vector<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N, other.data_, other.data_ + N, std::not_equal_to<T>());
	}

	bool operator <=(const Vector<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N, other.data_, other.data_ + N, std::less_equal<T>());
	}

	bool operator <(const Vector<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N, other.data_, other.data_ + N, std::less<T>());
	}

	bool operator >=(const Vector<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N, other.data_, other.data_ + N, std::greater_equal<T>());
	}

	bool operator >(const Vector<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N, other.data_, other.data_ + N, std::greater<T>());
	}

	T dot(const Vector<T,N>& other) const {
		return std::inner_product(data_, data_ + N, other.data_, 0);
	}

	Vector<T,N> operator *(const T& value) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, ret.data_, std::bind1st(std::multiplies<T>(), value));
		return ret;
	}

	Vector<T,N> operator *(const Vector<T,N>& other) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, other.data_, ret.data_, std::multiplies<T>());
		return ret;
	}

	Vector<T,N> operator +(const T& value) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, ret.data_, std::bind1st(std::plus<T>(), value));
		return ret;
	}

	Vector<T,N> operator +(const Vector<T,N>& other) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, other.data_, ret.data_, std::plus<T>());
		return ret;
	}

	Vector<T,N> operator -(const T& value) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, ret.data_, std::bind2nd(std::minus<T>(), value));
		return ret;
	}

	Vector<T,N> operator -(const Vector<T,N>& other) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, other.data_, ret.data_, std::minus<T>());
		return ret;
	}

	Vector<T,N> operator /(const T& value) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, ret.data_, std::bind2nd(std::divides<T>(), value));
		return ret;
	}

	Vector<T,N> operator /(const Vector<T,N>& other) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, other.data_, ret.data_, std::divides<T>());
		return ret;
	}

	Vector<T,N> operator %(const T& value) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, ret.data_, std::bind2nd(std::modulus<T>(), value));
		return ret;
	}

	Vector<T,N> operator %(const Vector<T,N>& other) const {
		Vector<T,N> ret;
		std::transform(data_, data_ + N, other.data_, ret.data_, std::modulus<T>());
		return ret;
	}

	Vector<T,N>& operator *=(const T& value) {
		std::transform(data_, data_ + N, data_, std::bind1st(std::multiplies<T>(), value));
		return *this;
	}

	Vector<T,N>& operator *=(const Vector<T,N>& other) {
		std::transform(data_, data_ + N, other.data_, data_, std::multiplies<T>());
		return *this;
	}

	Vector<T,N>& operator +=(const T& value) {
		std::transform(data_, data_ + N, data_, std::bind1st(std::plus<T>(), value));
		return *this;
	}

	Vector<T,N>& operator +=(const Vector<T,N>& other) {
		std::transform(data_, data_ + N, other.data_, data_, std::plus<T>());
		return *this;
	}

	Vector<T,N>& operator -=(const T& value) {
		std::transform(data_, data_ + N, data_, std::bind2nd(std::minus<T>(), value));
		return *this;
	}

	Vector<T,N>& operator -=(const Vector<T,N>& other) {
		std::transform(data_, data_ + N, other.data_, data_, std::minus<T>());
		return *this;
	}

	Vector<T,N>& operator /=(const T& value) {
		std::transform(data_, data_ + N, data_, std::bind2nd(std::divides<T>(), value));
		return *this;
	}

	Vector<T,N>& operator /=(const Vector<T,N> other) {
		std::transform(data_, data_ + N, other.data_, data_, std::divides<T>());
		return *this;
	}

	Vector<T,N>& operator %=(const T& value) {
		std::transform(data_, data_ + N, data_, std::bind2nd(std::modulus<T>(), value));
		return *this;
	}

	Vector<T,N>& operator %=(const Vector<T,N> other) {
		std::transform(data_, data_ + N, other.data_, data_, std::modulus<T>());
		return *this;
	}


	// GL coordinates
	T& x;
	T& y;
	T& z;
	T& w;
	
	// GL colours + alpha
	T& r;
	T& g;
	T& b;
	T& a;

	// GL texture coordinates
	T& s;
	T& t;
	T& p;
	T& q;
private:
	T data_[N];

	struct square_and_add {
		T operator ()(const T& a, const T&b) {
			return a + b*b;
		}
	};

};


typedef Vector<float,2> vec2;
Q_DECLARE_METATYPE(vec2);
Q_DECLARE_METATYPE(vec2*);
typedef Vector<float,3> vec3;
Q_DECLARE_METATYPE(vec3);
Q_DECLARE_METATYPE(vec3*);
typedef Vector<float,4> vec4;
Q_DECLARE_METATYPE(vec4);
Q_DECLARE_METATYPE(vec4*);

template <typename T, unsigned int N>
QDebug operator <<(QDebug dbg, const Vector<T,N>& v) {
	dbg.nospace() << "[";
	for (unsigned int i = 0; i < N-1; ++i)
		dbg.nospace() << v[i] << ",";

	dbg.nospace() << v[N-1] << "]";

	return dbg.space();
}

template <typename T, unsigned int N>
QDataStream& operator <<(QDataStream& stream, const Vector<T,N>& v) {
	for (unsigned int i = 0; i < N; ++i)
		stream << v[i];

	return stream;
}

template <typename T, unsigned int N>
QDataStream& operator >>(QDataStream& stream, Vector<T,N>& v) {
	for (unsigned int i = 0; i < N; ++i)
		stream >> v[i];

	return stream;
}

template <typename T, unsigned int N>
const Vector<T,N> operator /(const T& value, const Vector<T,N>& v) {
	Vector<T,N> ret;
	std::transform(v.data(), v.data() + N, ret.data(), std::bind1st(std::divides<T>(), value));
	return ret;
}


#undef INITIALISATION

#endif
