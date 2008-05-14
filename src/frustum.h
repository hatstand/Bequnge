#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <svl/SVL.h>


class Plane
{
public:
	void normalize()
	{
		float t = len(n);
		n /= t;
		d /= t;
	}
	
	float distanceTo(const Vec3& point) const
	{
		return dot(n, point) + d;
	}
	
	Vec3 n;
	float d;
};



class Frustum
{
public:
	void getPlaneEquations();
	
	bool testSphere(const Vec3& center, float radius);

private:
	enum PlaneName
	{
		RIGHT,
		LEFT,
		BOTTOM,
		TOP,
		PNEAR,
		PFAR
	};
	
	Plane frustumPlane[6];
};

#endif
