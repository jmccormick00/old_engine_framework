//======================================================================
// C_AABB.h 
// Author: James McCormick
// Discription:
//	An axially aligned bounding box.
//======================================================================

#ifndef _CAABB_
#define _CAABB_

#include "3DMath.h"

class C_AABB {
public:
	Math::vector3 d_min, d_max, d_center;

	C_AABB() {}
	C_AABB(const C_AABB& aabb) {
		d_min = aabb.d_min;
		d_max = aabb.d_max;
		d_center = aabb.d_center;
	}

	void calculate(Math::vector3 *v, int numPoints) {
		if(v == 0) return;

		for(int i = 0; i < numPoints; ++i) {
			if(v[i].x < d_min.x) d_min.x = v[i].x;
			if(v[i].x > d_max.x) d_max.x = v[i].x;

			if(v[i].y < d_min.y) d_min.y = v[i].y;
			if(v[i].y > d_max.y) d_max.y = v[i].y;

			if(v[i].z < d_min.z) d_min.z = v[i].z;
			if(v[i].z > d_max.z) d_max.z = v[i].z;
		}
		d_center.x = (d_min.x + d_max.x) * 0.5f;
		d_center.y = (d_min.y + d_max.y) * 0.5f;
		d_center.z = (d_min.z + d_max.z) * 0.5f;
	}
};


#endif // _CAABB_
