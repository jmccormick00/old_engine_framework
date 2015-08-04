//======================================================================
// C_Scene.cpp 
// Author: James McCormick
// Discription:
//	The implementation of the scene module
//======================================================================

#include "C_Scene.h"
#include "UtilityMacros.h"
#include <string>

// SceneManager=========================================================
I_SceneManager* G_SceneManager;
C_SceneManager C_SceneManager::d_instance;

C_Scene::C_Scene() {
	D3DXCreateMatrixStack(0, &d_pMatrixStack);
}

C_Scene::~C_Scene() {
	ReleaseCOM(d_pMatrixStack);
}


LightID C_Scene::addLight(C_Light* l) {
	// TODO - implement
	return 0;
}

CameraID C_Scene::addCamera(CameraPtr c) {
	// TODO - implement
	return 0;
}

CameraPtr C_Scene::getCurrentCamera() {
	// TODO - implement
	return NULL;
}

void C_Scene::getCamera(CameraID id) {
	// TODO - implement
}

C_Light* C_Scene::getLight(LightID id) {
	// TODO - implement
	return NULL;
}

void C_Scene::removeCamera(CameraPtr c) {
	// TODO - implement
}

void C_Scene::removeLight(LightID l) {
	// TODO - implement
}

void C_Scene::setCurrentCamera(CameraID id) {
	// TODO - implement
}
	
void C_Scene::onUpdate(const double& timeElapsed) {
	// TODO - implement
}

void C_Scene::onRender() {
	// TODO - implement
}

void C_Scene::addNode(NodePtr n) {
	// TODO - implement
}

void C_Scene::removeNode(NodePtr n) {
	// TODO - implement
}

void C_Scene::clean() {
}

C_SceneManager::C_SceneManager() : d_Scene(0) {
	G_SceneManager = this;
}

void C_SceneManager::setCurrentScene(ScenePtr s) {
	d_Scene = s;
}

ScenePtr C_SceneManager::getCurrentScene() {
	return d_Scene;
}

bool C_SceneManager::hasScene() {
	return (d_Scene != 0);
}
// SceneManager=========================================================


// Frustrum=============================================================
void C_Frustrum::buildFrustrum(float screenDepth, Math::matrix4x4 proj, Math::matrix4x4 view) {  // TODO - Fix this
	float zMinimum, r;
	D3DXMATRIX matrix;
	
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -proj._43 / proj._33;
	r = screenDepth / (screenDepth - zMinimum);
	proj._33 = r;
	proj._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	D3DXMatrixMultiply(&matrix, &view, &proj);

	// Calculate near plane of frustum.
	d_Planes[0].a = matrix._14 + matrix._13;
	d_Planes[0].b = matrix._24 + matrix._23;
	d_Planes[0].c = matrix._34 + matrix._33;
	d_Planes[0].d = matrix._44 + matrix._43;
	d_Planes[0].normalize();

	// Calculate far plane of frustum.
	d_Planes[1].a = matrix._14 - matrix._13; 
	d_Planes[1].b = matrix._24 - matrix._23;
	d_Planes[1].c = matrix._34 - matrix._33;
	d_Planes[1].d = matrix._44 - matrix._43;
	d_Planes[1].normalize();

	// Calculate left plane of frustum.
	d_Planes[2].a = matrix._14 + matrix._11; 
	d_Planes[2].b = matrix._24 + matrix._21;
	d_Planes[2].c = matrix._34 + matrix._31;
	d_Planes[2].d = matrix._44 + matrix._41;
	d_Planes[2].normalize();

	// Calculate right plane of frustum.
	d_Planes[3].a = matrix._14 - matrix._11; 
	d_Planes[3].b = matrix._24 - matrix._21;
	d_Planes[3].c = matrix._34 - matrix._31;
	d_Planes[3].d = matrix._44 - matrix._41;
	d_Planes[3].normalize();

	// Calculate top plane of frustum.
	d_Planes[4].a = matrix._14 - matrix._12; 
	d_Planes[4].b = matrix._24 - matrix._22;
	d_Planes[4].c = matrix._34 - matrix._32;
	d_Planes[4].d = matrix._44 - matrix._42;
	d_Planes[4].normalize();

	// Calculate bottom plane of frustum.
	d_Planes[5].a = matrix._14 + matrix._12;
	d_Planes[5].b = matrix._24 + matrix._22;
	d_Planes[5].c = matrix._34 + matrix._32;
	d_Planes[5].d = matrix._44 + matrix._42;
	d_Planes[5].normalize();
}

bool C_Frustrum::checkPoint(const Math::vector3& point) {
	for(int i = 0; i < 6; ++i) {
		if(D3DXPlaneDotCoord(&d_Planes[i], &point) < 0.0f) 
			return false;
	}
	return true;
}

// TODO - implement
bool C_Frustrum::checkRectangle(const Math::vector3& b1, const Math::vector3& b2) {
	return true;
}

// TODO - implement
bool C_Frustrum::checkSphere(const Math::vector3& center, float radius) {
	return true;
}


//bool C_Frustrum::checkAABB(AABBPtr box) {
//	return true;
//}
// Frustrum=============================================================
