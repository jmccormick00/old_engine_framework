
#include "OrbitCamera3D.h"

//------------------------------------------------------------------------------
// ZoomCamera()
// Moves the camera forward and backwards
//------------------------------------------------------------------------------
void C_OrbitCamera3D::ZoomCamera(float amount)
{
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	float length = d_vecLook.Magnitude();
	d_vecLook.Normalize();
	
	d_vecLook *= amount;
	float len2 = d_vecLook.Magnitude();

	if( amount > 0.0 )
	{
		if( (length - len2) > 1.0 )
			d_vecCamPos += d_vecLook;
	}
	else
		d_vecCamPos += d_vecLook;
}


//------------------------------------------------------------------------------
// OrbitX()
// Orbits the camera on the camera's X axis around the camera's target
//------------------------------------------------------------------------------
void C_OrbitCamera3D::OrbitX(float amount)
{
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	
	float r = d_vecLook.Magnitude();
	float w = r - (r * cos(amount));
	float h = r * sin(amount);
	
	d_vecLook.Normalize();
	d_vecCamPos += ((d_vecLook * w) + (d_vecCamRightVector * h)); 
	
	// Update the side Vector
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	d_vecCamRightVector = d_vecLook.CrossProduct(d_vecCamUpVector);
	d_vecCamRightVector.Normalize();
}


//------------------------------------------------------------------------------
// OrbitY()
// Orbits the camera on the camera's Y axis around the camera's target
//------------------------------------------------------------------------------
void C_OrbitCamera3D::OrbitY(float amount)
{
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	
	float r = d_vecLook.Magnitude();
	float w = r - (r * cos(amount));
	float h = r * sin(amount);
	
	d_vecLook.Normalize();
	d_vecCamPos += ((d_vecLook * w) + (d_vecCamUpVector * h));
	
	// Update the up vector
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	d_vecCamUpVector = d_vecCamRightVector.CrossProduct(d_vecLook);
	d_vecCamUpVector.Normalize();
}


