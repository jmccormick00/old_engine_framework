/*=========================================================================
/ James McCormick - Geometry.cpp
/ Defines some basic geometry in 3D space
/==========================================================================*/

#include "Geometry.h"

bool C_Plane::inside(const Math::vector3 &point) const {
	float result = D3DXPlaneDotCoord(this, &point);
	return (result >= 0.0f);
}

bool C_Plane::inside(const Math::vector3 &point, const float radius) const {
	float fDist;
	fDist = D3DXPlaneDotCoord(this, &point);
	return (fDist >= -radius);
}
