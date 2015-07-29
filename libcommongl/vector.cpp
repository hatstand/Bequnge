#include "vector.h"

template<>
Vector<float,3> Vector<float,3>::interpolate(
		const Vector<float,3>& tl, const Vector<float,3>& tr,
		const Vector<float,3>& bl, const Vector<float,3>& br,
		float weightX, float weightY) {
	float cWeightX = 1.0f - weightX;
	float cWeightY = 1.0f - weightY;

	return Vector<float,3>(::interpolate(tl.x, tr.x, bl.x, br.x,
										 weightX, weightY, cWeightX, cWeightY),
						   ::interpolate(tl.y, tr.y, bl.y, br.y,
										 weightX, weightY, cWeightX, cWeightY),
						   ::interpolate(tl.z, tr.z, bl.z, br.z,
										 weightX, weightY, cWeightX, cWeightY));
}

