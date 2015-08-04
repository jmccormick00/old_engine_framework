/*=========================================================================
/ James McCormick - Geometry.h
/ Defines some basic geometry in 3D space
/==========================================================================*/

#ifndef _GEOMETRY_
#define _GEOMETRY_

#include "3DMath.h"

class C_Plane : public D3DXPLANE {
public:
	inline void normalize();
	// normal faces away from you if you send in verts in counter clockwise order....
	inline void init(const Math::vector3 &p0, const Math::vector3 &p1, const Math::vector3 &p2);
	bool inside(const Math::vector3 &point, const float radius) const;
	bool inside(const Math::vector3 &point) const;
};

inline void C_Plane::normalize() {
	float mag;
	mag = sqrt(a*a + b*b + c*c);
	a /= mag;
	b /= mag;
	c /= mag;
	d /= mag;
}

inline void C_Plane::init(const Math::vector3 &p0, const Math::vector3 &p1, const Math::vector3 &p2) {
	D3DXPlaneFromPoints(this, &p0, &p1, &p2);
	normalize();
}

#endif // _GEOMETRY_
