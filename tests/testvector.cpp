#include "testvector.h"
#include "vector.h"

QTEST_MAIN(TestVector);

void TestVector::testConstructors()
{
	Vector<int,2> v1(2,3);
	QVERIFY(v1.x == 2);
	QVERIFY(v1.x == v1.r);
	QVERIFY(v1.x == v1.u);
	QVERIFY(v1.r == v1.u);
	QVERIFY(v1.y == 3);
	QVERIFY(v1.r == 2);
	QVERIFY(v1.g == 3);
	QVERIFY(v1.u == 2);
	QVERIFY(v1.v == 3);
	QVERIFY(v1[0] == 2);
	QVERIFY(v1[1] == 3);

	Vector<int,3> v2(2,3,4);
	QVERIFY(v2.x == 2);
	QVERIFY(v2.y == 3);
	QVERIFY(v2.z == 4);

	Vector<int,4> v3(2,3,4,5);
	QVERIFY(v3.x == 2);
	QVERIFY(v3.y == 3);
	QVERIFY(v3.z == 4);
	QVERIFY(v3.w == 5);

	Vector<int,2> v4(v1);
	QVERIFY(v1 == v4);
	QVERIFY(v1[0] == v4[0]);

	int array[2] = { 2,3 };
	Vector<int,2> v5(array);
	QVERIFY(v5.x == 2);
	QVERIFY(v5.y == 3);
}

void TestVector::testAssignment()
{
	Vector<int,2> v1(2,3);
	Vector<int,2> v2 = v1;

	QVERIFY(v1 == v2);
	QVERIFY(v1.x == v2.x);
	QVERIFY(v1[0] == v2[0]);
	v2.x = 7;
	QVERIFY(v1 != v2);
	QVERIFY(v1.x != v2.x);
	QVERIFY(v1.y == v2.y);
	QVERIFY(v2.r == v2.x);
	QVERIFY(v1.r == v1.x);
}

void TestVector::testOperators()
{
	Vector<int,2> v1(2,3);
	Vector<int,2> v2(4,5);

	// Test Comparisons
	QVERIFY(v1 != v2);
	QVERIFY(!(v1 == v2));

	// Test arithmetic
	Vector<int,2> expected_mult(8, 15);
	QVERIFY(v1 * v2 == expected_mult);
	Vector<int,2> expected_div(2, 1);
	QVERIFY(v2 / v1 == expected_div);
	Vector<int,2> expected_add(6, 8);
	QVERIFY(v1 + v2 == expected_add);
	Vector<int,2> expected_sub(2, 2);
	QVERIFY(v2 - v1 == expected_sub);
	Vector<int,2> expected_mod(0, 2);
	QVERIFY(v2 % v1 == expected_mod);

	Vector<int,2> muted = v1;
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
