//======================================================================
// Interface.h 
// Author: James McCormick
// Discription:
//	Contains the inferfaces of many of the key classes in the engine.
//======================================================================
#ifndef _INTERFACES_
#define _INTERFACES_




//-----------------------------------------------------------------------

enum E_ViewType
{
	View_Human,
	View_Remote,
	View_AI,
	View_Recorder
};

typedef unsigned int ViewID;

class I_View : public C_SmartPointerObject
{
public:
	virtual void OnRestore() = 0;
	virtual void OnRender(double time, float elapsedTime) = 0;
	virtual E_ViewType GetType() = 0;
	virtual ViewID GetID() = 0;
//	virtual void OnAttach(ViewID id, ActorID optionalID) = 0;

	//virtual bool OnMsgProc(MSG msg) = 0;
	virtual void OnUpdate(int deltaMilliseconds) = 0;

	virtual ~I_View() { };
};

class C_Resource;

class I_ResourceFile
{
public:
	virtual bool Open()=0;
	virtual int GetResourceSize(const C_Resource &r) = 0;
	virtual int GetResource(const C_Resource &r, char *buffer) = 0;
	virtual ~I_ResourceFile() { }
};


#endif // _INTERFACES_