//======================================================================
// I_EngineInterface.h 
// Author: James McCormick
// Discription:
//	Contains the inferfaces of many of the core classes for the engine.
//======================================================================
#ifndef _ENGINEINTERFACE_
#define _ENGINEINTERFACE_

#include "C_SmartPointer.h"
//#include <string>
#include <vector>


//-----------------------------------------------------------------------
// The EngineState interface
typedef unsigned StateID;  // A globally unique state ID number
class I_EngineState : public C_SmartPointerObject {
protected:
	StateID d_ID;

	virtual void setUpScene() = 0;
	virtual void setUpGUI() = 0;
	virtual void setUpEngine() = 0;
	virtual void cleanUpEngine() = 0;
	virtual void cleanUpScene() = 0;
	virtual void cleanUpGUI() = 0;

public:
	~I_EngineState() {}
	virtual const StateID& getID() const { return d_ID; }
	virtual void exit() {
		cleanUpEngine();
		cleanUpScene();
		cleanUpGUI();
	}
	virtual void enter() {
		setUpEngine();
		setUpScene();
		setUpGUI();
	}
};
typedef C_SmartPointer<I_EngineState> EngineStatePtr;
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// The System interface
typedef unsigned long SystemID;	// A globally unique system ID number
class I_EngineSystem : public C_SmartPointerObject {
protected:
	SystemID d_ID;
	bool d_bIsPaused;
	bool d_bIsVisible;

public:
	~I_EngineSystem() {}
	virtual void onRender() = 0;
	virtual void pause() { d_bIsPaused = true; }
	virtual void unPause() { d_bIsPaused = false; }
	virtual bool isVisible() const { return d_bIsVisible; }
	virtual bool isPaused() { return d_bIsPaused; }
	virtual void setVisibility(bool b) { d_bIsVisible = b; }
	virtual const SystemID& getID() const { return d_ID; }
	virtual void onUpdate(const double& elapsedTime) = 0;
	//virtual void onPreRender(const double& elapsedTime) = 0;
};
typedef C_SmartPointer<I_EngineSystem> EngineSystemPtr;
typedef std::vector<EngineSystemPtr> SystemList;
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// The messaging interface
typedef unsigned int MessageType;
class I_Message : public C_SmartPointerObject {
	const double d_dTimeStamp;
	const MessageType d_MessageType;

public:
	I_Message(const MessageType& type, const double& stamp) 
		: d_MessageType(type), d_dTimeStamp(stamp) {}
	virtual ~I_Message() {}

	virtual const double& getTimeStamp() const { return d_dTimeStamp; }
	virtual const MessageType& getType() const { return d_MessageType; }
};
typedef C_SmartPointer<I_Message> MessagePtr;

class I_MessageListener : public C_SmartPointerObject {
public:
	virtual ~I_MessageListener() {} 
	virtual bool onMessage(MessagePtr msg) = 0;
};
typedef C_SmartPointer<I_MessageListener> MsgListenerPtr;
//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
// The interface for the engine layer.
class I_Engine {
public:
	virtual ~I_Engine() {}

	virtual const double getTimeStamp() = 0;
	virtual void clean() = 0;

	// Engine Control interface
	virtual void tick() = 0;
	virtual void pause() = 0;
	virtual void unPause() = 0;
	virtual void clearSystems() = 0;
	virtual void clearSystemLists() = 0;
	virtual SystemList& getUpdateList() = 0;
	virtual SystemList& getRenderList() = 0;
	virtual void deleteSystem(const SystemID& id) = 0;
	virtual void addSystem(EngineSystemPtr sys) = 0;
	virtual EngineSystemPtr getSystem(const SystemID& id) = 0;

	// Engine's Finite State interface
	virtual void revertToPreviousState() = 0;
	virtual EngineStatePtr getCurrentState() = 0;
	virtual EngineStatePtr getPreviousState() = 0;
	virtual bool isInState(const StateID& state) = 0;
	virtual void setCurrentState(EngineStatePtr curState) = 0;
	virtual void queueStateChange(EngineStatePtr curState) = 0;
	virtual void setPreviousState(EngineStatePtr prevState) = 0;

	// Message Interface
	enum MsgStatus {
		E_NOTCONSUMED,
		E_CONSUMED,
		E_NOLISTENER,
		E_SUCCESS
	};

	virtual MsgStatus queueMessage(MessagePtr msg) = 0;
	virtual MsgStatus triggerMessage(MessagePtr msg) = 0;
	virtual bool addWildCardListener(MsgListenerPtr l) = 0;
	virtual void deleteWildCardListener(MsgListenerPtr l) = 0;
	virtual bool addListener(MsgListenerPtr l, const MessageType& type) = 0;
	virtual bool deleteListener(MsgListenerPtr l, const MessageType& type) = 0;
};

extern I_Engine* G_Engine;
//-----------------------------------------------------------------------

#endif // _ENGINEINTERFACE_
