//======================================================================
// I_SceneInterface.h 
// Author: James McCormick
// Discription:
//	The interfaces for the Scene module
//======================================================================

#ifndef _ISCENEINTERFACE_
#define _ISCENEINTERFACE_

#include "C_SmartPointer.h"
#include "C_Light.h"
#include "I_Effect.h"

class I_BaseObject;
typedef C_SmartPointer<I_BaseObject> ObjectPtr;

typedef unsigned CameraID;
class I_Camera {
public:
	~I_Camera() {}
	
	Math::vector3 d_up;
	Math::vector3 d_pos;
	Math::vector3 d_lookAt;
};
typedef C_SmartPointer<I_Camera> CameraPtr;

class I_SceneNode : public C_SmartPointerObject {
public:
	~I_SceneNode() {}

};
typedef C_SmartPointer<I_SceneNode> NodePtr;

class I_Scene : public C_SmartPointerObject {
public:
	~I_Scene() {}

	virtual LightID addLight(C_Light* l) = 0;
	virtual CameraID addCamera(CameraPtr c) = 0;
	virtual CameraPtr getCurrentCamera() = 0;
	virtual void getCamera(CameraID id) = 0;
	virtual C_Light* getLight(LightID id) = 0;

	virtual void removeCamera(CameraPtr c) = 0;
	virtual void removeLight(LightID l) = 0;

	virtual void popMatrix() = 0;
	virtual void pushMatrix(const Math::matrix4x4& m) = 0;
	virtual const Math::matrix4x4* getTopMatrix() = 0;

	virtual void setCurrentCamera(CameraID id) = 0;
	virtual void onUpdate(const double& timeElapsed) = 0;
	virtual void onRender() = 0;

	virtual void addNode(NodePtr n) = 0;
	virtual void removeNode(NodePtr n) = 0;

	// Finds an object that a ray hits.  Returns null if none
	// TODO - virtual ObjectPtr castRayIntersect(const ray& r) = 0;
};
typedef C_SmartPointer<I_Scene> ScenePtr;

class I_SceneManager {
public:
	~I_SceneManager() {}
	virtual void setCurrentScene(ScenePtr s) = 0;
	virtual ScenePtr getCurrentScene() = 0;
	virtual bool hasScene() = 0;
	
	virtual EffectPtr getEffect(const HashedValue& id) = 0;
	// Returns false if the registery failed, check the log to find out more detail
	virtual bool registerEffect(const HashedValue& name, EffectPtr e) = 0;
	virtual void removeEffect(const HashedValue& id) = 0;
};
extern I_SceneManager* G_SceneManager;

#endif // _ISCENEINTERFACE_
