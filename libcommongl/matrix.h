#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <functional>
#include <numeric>

#include <QtDebug>

#include "vector.h"
#include "opengl.h"

template <typename T = float, unsigned int N = 4>
class Matrix {
public:
	Matrix() {
		std::fill(data_, data_ + N*N, 0);
	}

	Matrix(const T* array) {
		std::copy(array, array + N*N, data_);
	}

	Matrix(const Matrix<T,N>& other) {
		std::copy(other.data_, other.data_ + N*N, data_);
	}

	Matrix(GLenum e);

	void loadMatrix();

	Vector<T,N> operator [](unsigned int i) const {
		return Vector<T,N>(data_ + N*i);
	}

	// Column major.
	T& at(unsigned int col, unsigned int row) {
		return *(data_ + col*N + row);
	}

	const T& at(unsigned int col, unsigned int row) const {
		return *(data_ + col*N + row);
	}

	const T* data() const {
		return data_;
	}

	T* data() {
		return data_;
	}

	Matrix<T,N>& operator =(const Matrix<T,N>& other) {
		std::copy(other.data_, other.data_ + N*N, data_);
		return *this;
	}

	Matrix<T,N>& operator =(const T* array) {
		std::copy(array, array + N*N, data_);
	}

	bool operator ==(const Matrix<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N*N, other.data_, other.data_ + N*N, std::equal_to<T>());
	}

	bool operator !=(const Matrix<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N*N, other.data_, other.data_ + N*N, std::not_equal_to<T>());
	}

	bool operator <=(const Matrix<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N*N, other.data_, other.data_ + N*N, std::less_equal<T>());
	}

	bool operator <(const Matrix<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N*N, other.data_, other.data_ + N*N, std::less<T>());
	}

	bool operator >=(const Matrix<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N*N, other.data_, other.data_ + N*N, std::greater_equal<T>());
	}

	bool operator >(const Matrix<T,N>& other) const {
		return std::lexicographical_compare(
			data_, data_ + N*N, other.data_, other.data_ + N*N, std::greater<T>());
	}

	Matrix<T,N>& transpose() {
		for (unsigned int i = 0; i < N; ++i) {
			for (unsigned int j = i; j < N; ++j) {
				std::swap(data_[j*N + i], data_[i*N + j]);
			}
		}

		return *this;
	}

	Matrix<T,N> operator *(const T& value) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, ret.data_, std::bind1st(std::multiplies<T>(), value));
		return ret;
	}

	Matrix<T,N> operator *(const Matrix<T,N>& other) const {
		Matrix<T,N> ret;
		for (unsigned int i = 0; i < N; ++i) {
			for (unsigned int j = 0; j < N; ++j) {
				for (unsigned int k = 0; k < N; ++k)
					ret.data_[j*N + i] += data_[k*N + i] * other.data_[j*N + k];
			}
		}
		return ret;
	}

	Matrix<T,N> operator +(const T& value) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, ret.data_, std::bind1st(std::plus<T>(), value));
		return ret;
	}

	Matrix<T,N> operator +(const Matrix<T,N>& other) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, other.data_, ret.data_, std::plus<T>());
		return ret;
	}

	Matrix<T,N> operator -(const T& value) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, ret.data_, std::bind2nd(std::minus<T>(), value));
		return ret;
	}

	Matrix<T,N> operator -(const Matrix<T,N>& other) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, other.data_, ret.data_, std::minus<T>());
		return ret;
	}

	Matrix<T,N> operator /(const T& value) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, ret.data_, std::bind2nd(std::divides<T>(), value));
		return ret;
	}

	Matrix<T,N> operator /(const Matrix<T,N>& other) const {
		Matrix<T,N> ret;
		for (unsigned int i = 0; i < N; ++i) {
			for (unsigned int j = 0; j < N; ++j) {
				for (unsigned int k = 0; k < N; ++k)
					ret.data_[j*N + i] += data_[k*N + i] / other.data_[j*N + k];
			}
		}
		return ret;
	}

	Matrix<T,N> operator %(const T& value) const {
		Matrix<T,N> ret;
		std::transform(data_, data_ + N*N, ret.data_, std::bind2nd(std::modulus<T>(), value));
		return ret;
	}

	Matrix<T,N> operator %(const Matrix<T,N>& other) const {
		Matrix<T,N> ret;
		for (unsigned int i = 0; i < N; ++i) {
			for (unsigned int j = 0; j < N; ++j) {
				for (unsigned int k = 0; k < N; ++k)
					ret.data_[j*N + i] += data_[k*N + i] % other.data_[j*N + k];
			}
		}
		return ret;
	}

	Matrix<T,N>& operator *=(const T& value) {
		std::transform(data_, data_ + N*N, data_, std::bind1st(std::multiplies<T>(), value));
		return *this;
	}

	Matrix<T,N>& operator *=(const Matrix<T,N>& other) {
		return *this = *this * other;
	}

	Matrix<T,N>& operator +=(const T& value) {
		std::transform(data_, data_ + N*N, data_, std::bind1st(std::plus<T>(), value));
		return *this;
	}

	Matrix<T,N>& operator +=(const Matrix<T,N>& other) {
		std::transform(data_, data_ + N*N, other.data_, data_, std::plus<T>());
		return *this;
	}

	Matrix<T,N>& operator -=(const T& value) {
		std::transform(data_, data_ + N*N, data_, std::bind2nd(std::minus<T>(), value));
		return *this;
	}

	Matrix<T,N>& operator -=(const Matrix<T,N>& other) {
		std::transform(data_, data_ + N*N, other.data_, data_, std::minus<T>());
		return *this;
	}

	Matrix<T,N>& operator /=(const T& value) {
		std::transform(data_, data_ + N*N, data_, std::bind2nd(std::divides<T>(), value));
		return *this;
	}

	Matrix<T,N>& operator /=(const Matrix<T,N> other) {
		return *this = *this / other;
	}

	Matrix<T,N>& operator %=(const T& value) {
		std::transform(data_, data_ + N*N, data_, std::bind2nd(std::modulus<T>(), value));
		return *this;
	}

	Matrix<T,N>& operator %=(const Matrix<T,N> other) {
		return *this = *this % other;
	}

private:
	T data_[N*N];
};

template <typename T, unsigned int N>
QDebug operator <<(QDebug dbg, const Matrix<T,N>& m) {
	dbg.nospace() << "(";
	for (unsigned int i = 0; i < N; ++i) {
		dbg.nospace() << "[";
		for (unsigned int j = 0; j < N; ++j)
			dbg.nospace() << m.at(i, j) << (j == N-1 ? "" : ",");

		dbg.nospace() << (i == N-1 ? "]" : "],");
	}
	dbg.nospace() << ")";

	return dbg.space();
}


typedef Matrix<float,2> mat2;
typedef Matrix<float,3> mat3;
typedef Matrix<float,4> mat4;

#endif
