#include "testvector.h"
#include "vector.h"

QTEST_MAIN(TestVector);

void TestVector::testOperators()
{
	Vector<int,2> v1(2,3);
	Vector<int,2> v2(4,5);

	// Test Comparisons
	QVERIFY(v1 != v2);
	QVERIFY(!(v1 == v2));

	// Test copy constructor
	Vector<int,2> v1_copy(v1);
	QVERIFY(v1 == v1_copy);
	// Test assignment
	Vector<int,2> v1_assign = v1;
	QVERIFY(v1 == v1_assign);

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
}
