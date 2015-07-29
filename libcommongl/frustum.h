#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "vector.h"

class Plane
{
public:
	void normalize()
	{
		float t = n.len();
		n /= t;
		d /= t;
	}
	
	float distanceTo(const vec3& point) const
	{
		return n.dot(point) + d;
	}
	
	vec3 n;
	float d;
};



class Frustum
{
public:
	void getPlaneEquations();
	
	bool testSphere(const vec3& center, float radius);

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
