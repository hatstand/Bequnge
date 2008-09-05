#include "testmatrix.h"
#include "matrix.h"
#include "opengl.h"
#include "vector.h"

#include <algorithm>

QTEST_MAIN(TestMatrix);

// GL Mock
void glGetFloatv(GLenum e, float* p) {
	float d[16] = {
		1,2,3,4,
		5,6,7,8,
		9,10,11,12,
		13,14,15,16
	};

	std::copy(d, d + 16, p);
}

void TestMatrix::testConstructors()
{
	int data[4] = { 1,2,3,4 };
	Matrix<int,2> v1(data);
	Vector<int,2> col1(1,2);
	Vector<int,2> col2(3,4);
	QVERIFY(v1[0] == col1);
	QVERIFY(v1[1] == col2);

	QVERIFY(v1.at(0,0) == 1);
	QVERIFY(v1.at(0,1) == 2);
}

void TestMatrix::testAssignment()
{
	int data[4] = { 1,2,3,4 };
	Matrix<int,2> v1(data);
	Matrix<int,2> v2 = v1;

	QVERIFY(v1 == v2);
	QVERIFY(v1[0] == v2[0]);
	int& a = v1.at(0, 0);
	++a;
	QVERIFY(v1 != v2);
	QVERIFY(v1[0] != v2[0]);
	QVERIFY(v1[1] == v2[1]);
}

void TestMatrix::testOperators()
{
	int data1[4] = { 1,2,3,4 };
	int data2[4] = { 5,6,7,8 };
	Matrix<int,2> v1(data1);
	Matrix<int,2> v2(data2);

	// Test Comparisons
	QVERIFY(v1 != v2);
	QVERIFY(!(v1 == v2));

	// Test arithmetic
	int expected_mult_data[4] = { 23,34,31,46 };
	Matrix<int,2> expected_mult(expected_mult_data);
	QVERIFY(v1 * v2 == expected_mult);

	int big_mult_data[16] = {
		1,2,3,4,
		5,6,7,8,
		9,10,11,12,
		13,14,15,16
	};
	int expected_big_mult_data[16] = {
		90,100,110,120,
		202,228,254,280,
		314,356,398,440,
		426,484,542,600
	};
	Matrix<int,4> expected_big_mult(expected_big_mult_data);
	Matrix<int,4> big_mult(big_mult_data);
	QVERIFY(big_mult * big_mult == expected_big_mult);

	int expected_other_mult_data[16] = {
		30,70,110,150,
		70,174,278,382,
		110,278,446,614,
		150,382,614,846
	};
	Matrix<int,4> expected_other_mult(expected_other_mult_data);
	Matrix<int,4> big_mult_trans(big_mult);
	big_mult_trans.transpose();
	Matrix<int,4> other_mult_result(big_mult_trans * big_mult);
	QVERIFY(other_mult_result == expected_other_mult);


	int expected_div_data[4] = { 0,1,0,0 };
	Matrix<int,2> expected_div(expected_div_data);
	QVERIFY(v1 / v2 == expected_div);

	int expected_add_data[4] = { 6,8,10,12 };
	Matrix<int,2> expected_add(expected_add_data);
	QVERIFY(v1 + v2 == expected_add);

	int expected_sub_data[4] = { 4,4,4,4 };
	Matrix<int,2> expected_sub(expected_sub_data);
	QVERIFY(v2 - v1 == expected_sub);

	int expected_int_mult_data[4] = { 2,4,6,8 };
	Matrix<int,2> expected_int_mult(expected_int_mult_data);
	QVERIFY(v1 * 2 == expected_int_mult);

	Matrix<int,2> muted = v1;
	QVERIFY((muted *= 2) == (v1 * 2));
	muted = v1;
	QVERIFY((muted *= v2) == (v1 * v2));
	muted = v1;
	QVERIFY((muted += 2) == (v1 + 2));
	muted = v1;
	QVERIFY((muted += v2) == (v1 + v2));
	muted = v1;
	QVERIFY((muted /= 2) == (v1 / 2));
	muted = v1;
	QVERIFY((muted /= v2) == (v1 / v2));
	muted = v1;
	QVERIFY((muted %= 2) == (v1 % 2));
	muted = v1;
	QVERIFY((muted %= v2) == (v1 % v2));
}

void TestMatrix::testTranspose() {
	int data[4] = { 1,2,3,4 };
	int result[4] = { 1,3,2,4 };
	Matrix<int,2> mat(data);
	Matrix<int,2> res(result);
	QVERIFY(mat.transpose() == res);

	float d[16] = {
		1,2,3,4,
		5,6,7,8,
		9,10,11,12,
		13,14,15,16
	};
	float r[16] = {
		1,5,9,13,
		2,6,10,14,
		3,7,11,15,
		4,8,12,16
	};
	Matrix<float,4> m(d);
	Matrix<float,4> r2(r);
	m.transpose();
	QVERIFY(m == r2);
		
}

void TestMatrix::testGl() {
	Matrix<float,4> m(GL_MODELVIEW_MATRIX);
	QVERIFY(m.at(0,0) == 1);
	QVERIFY(m.at(0,1) == 2);
}	


void TestMatrix::testSimpleOperators() {
	float data[4] = {1,2,3,4};
	float mult_result[4] = {2,4,6,8};
	float add_result[4] = {2,3,4,5};
	float min_result[4] = {0,1,2,3};
	float div_result[4] = {0.5,1,1.5,2};

	Matrix<float,2> mat(data);

	Matrix<float,2> res(add_result);
	QVERIFY(mat * 2 == res);

	res = Matrix<float,2>(add_result);
	QVERIFY(mat + 1 == res);

	res = Matrix<float,2>(min_result);
	QVERIFY(mat - 1 == res);

	res = Matrix<float,2>(div_result);
	QVERIFY(mat / 2 == res);
}
