//======================================================================
// C_Light.h 
// Author: James McCormick
// Discription:
//	The light structure
//======================================================================

#ifndef _CLIGHT_
#define _CLIGHT_

#include "3DMath.h"

typedef unsigned LightID;
struct C_Light {
	C_Light() {
		ZeroMemory(this, sizeof(C_Light));
	}

	Math::vector3 pos;
	float pad1;      // not used
	Math::vector3 dir;
	float pad2;      // not used
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	Math::vector3 att;
	float spotPow;
	float range;
};

#endif // _CLIGHT_
