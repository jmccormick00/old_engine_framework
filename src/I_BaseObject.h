//======================================================================
// I_BaseObject.h 
// Author: James McCormick
// Discription:
//	Contains the inferface for the base object in the engine
//======================================================================
#include "C_SmartPointer.h"

//-----------------------------------------------------------------------
// The base inteface for any object in the framework
typedef unsigned ObjectID;
typedef unsigned long ObjectType;

//-----------------------------------------------------------------------
// The base inteface for any object in the framework
typedef unsigned ObjectID;
typedef unsigned long ObjectType;

class I_BaseObject : public C_SmartPointerObject {
protected:
	friend class C_Engine;
	ObjectID d_ID;

public:
	virtual ~I_BaseObject() {}
	//virtual void SetMat(const matrix4x4& m) = 0;
	virtual void setID(const ObjectID& id) { d_ID = id; }
	//virtual matrix4x4 const & getWorldMat() = 0;
	virtual const ObjectID& getID() const { return d_ID; }
	virtual const ObjectType& getType() const = 0;
	virtual bool isPhysical() = 0;
	virtual void onUpdate(const double& deltaSeconds) = 0;
	virtual void onRender() = 0;
};
typedef C_SmartPointer<I_BaseObject> ObjectPtr;
