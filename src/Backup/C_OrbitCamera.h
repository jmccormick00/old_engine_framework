/*=========================================================================
/ James McCormick - OrbitCamera3D.h
/ A class for representing the camera in the 3D scene
/ The camera is not free roaming.  It fixed to look at a target.
/ Gives the user the ability to pan, orbit and zoom in relation to a target
/==========================================================================*/

#ifndef _CAMERA3D_
#define _CAMERA3D_

#define PI 3.14159265

class C_OrbitCamera3D
{
	private:
		C_Vector3 d_vecLook,		// Used by various functions for calculations	
				d_vecCamPos,		// Camera's position
				d_vecCamTarget,			// The place where the camera looks
				d_vecCamUpVector,		// The camera's up direction, the camera's Y axis
				d_vecCamRightVector,	// The Camera's side direction, the camera's X axis
				d_vecSceneUpVector;		// The scene's up vector
				
		float   d_vecViewAngle,		// The angle of view in the vertical of the camera (Degrees)
				d_vecNearDist,			// The distance of the camera to the screen
				d_vecAspectRatio;		// The ratio of the hight of the screen to the width (x/y)			
		
		// Updates the screen in relation to the camera
		void UpdateScreen();
		
	public:
		C_OrbitCamera3D();
		C_OrbitCamera3D(C_Vector3 pos, C_Vector3 target, C_Vector3 camUp, C_Vector3 sceneUp);	
		~C_OrbitCamera3D() {}
	
		// updates the camera for the scene
		void Update();
	
		// Set various components of the class
		void SetPosition(C_Vector3 pos);
		void SetTarget(C_Vector3 target);
		void SetUpVector(C_Vector3 up);
		void SetSceneUpVector(C_Vector3 up);
	
		// Get various compents from the class
		C_Vector3 GetPosition();
		C_Vector3 GetTarget();
		C_Vector3 GetUpVector();
		C_Vector3 GetSceneUpVector();
	
		// Pan the camera
		void PanHorizontal(float x);
		void PanVertical(float y);
	
		// Zoom the camera
		void ZoomCamera(float amount);
	
		// 3D orbit the camera around the target
		void OrbitX(float amount);
		void OrbitY(float amount);
		
};


//------------------------------------------------------------------------------
// Constructor
// Initialize the camera to position (0, 0, 1), target(0, 0, 0)
// UpVector(0, 1, 0), SceneUp(0, 1, 0)
//------------------------------------------------------------------------------
inline C_OrbitCamera3D::C_OrbitCamera3D()
{	
	d_vecCamPos[2] = 1.0f;
	d_vecCamPos[0] = d_vecCamPos[1] = 0.0f;
	
	d_vecCamTarget[0] = d_vecCamTarget[1] = d_vecCamTarget[2] = 0.0f;
	
	d_vecCamUpVector[1] = 1.0f;
	d_vecCamUpVector[0] = d_vecCamUpVector[2] = 0.0f;
	
	d_vecSceneUpVector[1] = 1.0f;
	d_vecSceneUpVector[0] = d_vecSceneUpVector[2] = 0.0f;
	
	// Calculate the side vector
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	d_vecCamRightVector = d_vecLook.CrossProduct(d_vecCamUpVector);
	d_vecCamRightVector.Normalize();
}

inline C_OrbitCamera3D::C_OrbitCamera3D(C_Vector3 pos, C_Vector3 target, C_Vector3 camUp, C_Vector3 sceneUp)
{	
	d_vecCamPos = pos;
	d_vecCamTarget = target;
	d_vecCamUpVector = camUp;
	d_vecSceneUpVector = sceneUp;
	
	d_vecSceneUpVector.Normalize();
	d_vecCamUpVector.Normalize();
	
	// Calculate the side vector
	d_vecLook = d_vecCamTarget - d_vecCamPos;
	d_vecCamRightVector = d_vecLook.CrossProduct(d_vecCamUpVector);
	d_vecCamRightVector.Normalize();
}


//------------------------------------------------------------------------------
// SetPosition()
// Sets the camera's postion
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::SetPosition(C_Vector3 pos)
{
	d_vecCamPos = pos;
}


//------------------------------------------------------------------------------
// SetTarget()
// Sets the camera's target
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::SetTarget(C_Vector3 target)
{
	d_vecCamTarget = target;
}


//------------------------------------------------------------------------------
// SetUpVector()
// Sets the camera's up vector
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::SetUpVector(C_Vector3 up)
{
	d_vecCamUpVector = up;
}


//------------------------------------------------------------------------------
// SetSceneUpVector()
// Sets the scene's up vector
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::SetSceneUpVector(C_Vector3 up)
{
	d_vecSceneUpVector = up;
}

//------------------------------------------------------------------------------
// GetPosition()
// Gets the camera's position
//------------------------------------------------------------------------------
inline C_Vector3 C_OrbitCamera3D::GetPosition()
{
	return d_vecCamPos;
}

//------------------------------------------------------------------------------
// GetTarget()
// Gets the camera's target
//------------------------------------------------------------------------------
inline C_Vector3 C_OrbitCamera3D::GetTarget()
{
	return d_vecCamTarget;
}

//------------------------------------------------------------------------------
// GetUpVector()
// Gets the camera's up vector
//------------------------------------------------------------------------------
inline C_Vector3 C_OrbitCamera3D::GetUpVector()
{
	return d_vecCamUpVector;
}


//------------------------------------------------------------------------------
// GetSceneUpVector()
// Gets the scene's up vector
//------------------------------------------------------------------------------
inline C_Vector3 C_OrbitCamera3D::GetSceneUpVector()
{
	return d_vecSceneUpVector;
}

//------------------------------------------------------------------------------
// Update()
// Sets the camera so that the scene is drawn
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::Update()
{
	gluLookAt(	d_vecCamPos[0], d_vecCamPos[1], d_vecCamPos[2], 
				d_vecCamTarget[0], d_vecCamTarget[1], d_vecCamTarget[2], 
				d_vecCamUpVector[0], d_vecCamUpVector[1], d_vecCamUpVector[2] );
}

//------------------------------------------------------------------------------
// PanHorizontal()
// Pans the camera horizontally
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::PanHorizontal(float x)
{	
	d_vecCamPos += d_vecCamRightVector * x;
	d_vecCamTarget += d_vecCamRightVector * x;
}


//------------------------------------------------------------------------------
// PanVertical()
// Pans the camera vertically
//------------------------------------------------------------------------------
inline void C_OrbitCamera3D::PanVertical(float y)
{	
	d_vecCamPos += d_vecCamUpVector * y;
	d_vecCamTarget += d_vecCamUpVector * y;
}

#endif
