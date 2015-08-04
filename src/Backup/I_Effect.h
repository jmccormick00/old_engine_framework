//======================================================================
// I_Effect.h 
// Author: James McCormick
// Discription:
//	The base class for the effect file
//======================================================================
#ifndef _IEFFECT_
#define _IEFFECT_

#include "C_SmartPointer.h"
#include "C_Light.h"
#include "C_HashedString.h"

class I_Effect : public C_SmartPointerObject {
protected:

public:
	enum EffectMatrixType { E_WORLD, E_PROJ, E_VIEW };
	~I_Effect() {}
	void init() {
		buildEffect();
		buildInputLayout();
	}
	virtual void onDeviceReset() = 0;
	virtual void buildEffect() = 0;
	virtual void buildInputLayout() = 0;
	virtual void sendLights(C_Light* lightArr, int numLights) = 0;
	virtual void sendMatrix(const Math::vector3& mat, EffectMatrixType t) = 0;
	virtual void sendData(void const* p, short dataCode) = 0;
};
typedef C_SmartPointer<I_Effect> EffectPtr;

#endif // _IEFFECT_
