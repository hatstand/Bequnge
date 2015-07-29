#include "matrix.h"

template<>
void Matrix<double,4>::loadMatrix() {
	glLoadMatrixd(data_);
}

template<>
void Matrix<float,4>::loadMatrix() {
	glLoadMatrixf(data_);
}

template<>
Matrix<double,4>::Matrix(GLenum e) {
	glGetDoublev(e, data_);
}

template<>
Matrix<float,4>::Matrix(GLenum e) {
	glGetFloatv(e, data_);
}
