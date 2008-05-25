#include "testmatrix.h"
#include "matrix.h"
#include "vector.h"

QTEST_MAIN(TestMatrix);

void TestMatrix::testConstructors()
{
	int data[4] = { 1,2,3,4 };
	Matrix<int,2> v1(data);
	Vector<int,2> col1(1,2);
	Vector<int,2> col2(3,4);
	QVERIFY(v1[0] == col1);
	QVERIFY(v1[1] == col2);
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
}

