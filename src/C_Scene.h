//======================================================================
// C_Scene.h 
// Author: James McCormick
// Discription:
//	The implementation of the scene module
//======================================================================

#ifndef _CSCENE_
#define _CSCENE_

#include "I_BaseObject.h"
#include "I_SceneInterface.h"
#include "Geometry.h"
#include "C_Light.h"
#include "T_Registry.h"
#include <vector>
#include <map>


class C_Frustrum : public C_SmartPointerObject {
private:
	C_Plane d_Planes[6];

public:
	C_Frustrum() {}
	~C_Frustrum() {}
	C_Frustrum(const C_Frustrum&);

	// Call this function before you render the frame to update the frustrum with the current camera
	// Send in world coordinates
	void buildFrustrum(float screenDepth, Math::matrix4x4 proj, Math::matrix4x4 view);
	bool checkPoint(const Math::vector3&);
	bool checkRectangle(const Math::vector3& , const Math::vector3& max);
	bool checkSphere(const Math::vector3& center, float radius);
	//bool checkAABB(AABBPtr box); TODO
};


class C_Scene : public I_Scene {
protected:
	ID3DXMatrixStack* d_pMatrixStack;
	std::vector<C_Light> d_vLights;

	// A quick way to gain access to BaseObjects in the scene
	T_Registry<ObjectID, I_SceneNode> d_BaseObjectMap;

public:
	C_Scene();
	~C_Scene();

	LightID addLight(C_Light* l);
	CameraID addCamera(CameraPtr c);
	CameraPtr getCurrentCamera();
	void getCamera(CameraID id);
	C_Light* getLight(LightID id);

	void removeCamera(CameraPtr c);
	void removeLight(LightID l);

	void popMatrix() {
		d_pMatrixStack->Pop();
	}

	void pushMatrix(const Math::matrix4x4& m) {
		d_pMatrixStack->Push();
		d_pMatrixStack->MultMatrixLocal(&m);
	}

	const Math::matrix4x4* getTopMatrix() {
		return static_cast<const Math::matrix4x4*>(d_pMatrixStack->GetTop()); 
	}

	void setCurrentCamera(CameraID id);
	void onUpdate(const double& timeElapsed);
	void onRender();

	void addNode(NodePtr n);
	void removeNode(NodePtr n);

	void clean();
};


class C_SceneManager : public I_SceneManager {
private:
	ScenePtr d_Scene;
	C_SceneManager();
	C_SceneManager(const C_SceneManager&);

	static C_SceneManager d_instance;

	// Stores a hash table of the effects.  The index into the hash table 
	// is a hashed string of the name of the effect
	T_Registry<HashedValue, I_Effect> d_effectMap; 

public:
	~C_SceneManager() {}
	void setCurrentScene(ScenePtr s);
	ScenePtr getCurrentScene();
	bool hasScene();
	
	EffectPtr getEffect(const HashedValue& id) {
		return d_effectMap.get(id);
	}
	
	// Returns false if the registery failed
	bool registerEffect(const HashedValue& id, EffectPtr e) {
		return (d_effectMap.insert(id, e) == d_effectMap.E_SUCCESS); 
	}
	void removeEffect(const HashedValue& id) {
		d_effectMap.remove(id); 
	}
};


#endif // _CSCENE_
