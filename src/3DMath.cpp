#include "3DMath.h"

Math::matrix4x4 Math::matrix4x4::G_Identity( D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) );
Math::quaternion Math::quaternion::G_Identity(D3DXQUATERNION(0,0,0,1));
