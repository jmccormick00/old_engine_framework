/*=========================================================================
/ James McCormick - C_Engine.h
/ A class for handling the core components
/==========================================================================*/

#ifndef _CENGINE_
#define _CENGINE_

#include "I_EngineInterface.h"
#include "C_Timer.h"
#include <list>
#include <map>

#define MAXSYSTEMS 10

class C_Engine : public I_Engine {
private:
	C_Engine();
	C_Engine(const C_Engine&);
	static C_Engine d_Engine;	// This is a singleton

	C_Timer d_timer;
	double d_dCurTimeStamp;

	// Engine systems
	typedef std::map<SystemID, EngineSystemPtr> SystemMap;
	SystemMap d_SystemMap;				// the map of systems and their ID's
	SystemList d_vUpdateList;			// the update list
	SystemList d_vRenderList;			// The render list
	//-------------------------------------

	// Engine's Finite State
	bool d_bIsPaused;
	EngineStatePtr d_pCurrentState;		// The current state of the engine
	EngineStatePtr d_pPreviousState;	// The previous state of the engine
	EngineStatePtr d_pQueuedState;		// The queued state to change to
	//-------------------------------------

	// Message Components
	// one list per message type (stored in the map)
	typedef std::list<MsgListenerPtr> ListenerList;	
	// mapping of Message ID to listener list
	typedef std::map<MessageType, ListenerList>	MessageListenerMap; 
	// An entry pair for the map
	typedef std::pair<MessageType, ListenerList> MessageListenerMapPair;
	// A map insertion result
	typedef std::pair<MessageListenerMap::iterator, bool> MessageListenerMapIResult;
	// queue of pending- or processing-events - double buffer queue, one takes new messages while one is being processed.
	typedef std::list<MessagePtr> MessageQueue;
	MessageQueue d_MessageQueue[2];								
	MessageListenerMap d_ListenerMap;
	bool d_bCurrentMsgQueue;											// the queue that is active
	ListenerList d_WildCardListenerList;									// The list of the wildcard listeners.
	//-------------------------------------

	void onRender();
	//void onPreRender(const double& elapsedTime);
	void onUpdate(const double& elapsedTime);
	void dispatchMessages();

public:
	virtual ~C_Engine();

	virtual void clean();

	void start() { d_timer.reset(); d_dCurTimeStamp = 0.0; }

	//-----------------------------------------------------------------------
	// pause - Public C_Engine
	// Description 
	//		Pauses the engine.  No more updates are performed, but draw is
	//		still called.
	//
	// Arguments:	None.
	// Returns:		None.
	//-----------------------------------------------------------------------
	virtual void pause() { d_bIsPaused = true; d_timer.pause(); }

	//-----------------------------------------------------------------------
	// unPause - Public C_Engine
	// Description 
	//		Unpauses the engine.  The engine now perform updates.
	//
	// Arguments:	None.
	// Returns:		None.
	//-----------------------------------------------------------------------
	virtual void unPause() { d_bIsPaused = false; d_timer.start(); }

	virtual const double getTimeStamp() { return d_dCurTimeStamp; }
	virtual void tick();

	virtual void clearSystems();
	virtual void clearSystemLists();

	//-----------------------------------------------------------------------
	// getUpdateList - Public C_Engine
	// Description 
	//		Returns the update list
	//
	// Arguments:	None.
	// Returns:		a reference to a system list
	//-----------------------------------------------------------------------
	virtual SystemList& getUpdateList() { return d_vUpdateList; }
	
	//-----------------------------------------------------------------------
	// getRenderList - Public C_Engine
	// Description 
	//		Returns the render list
	//
	// Arguments:	None.
	// Returns:		a reference to a system list
	//-----------------------------------------------------------------------
	virtual SystemList& getRenderList() { return d_vRenderList; }

	virtual void revertToPreviousState();
	virtual void deleteSystem(const SystemID& id);

	//-----------------------------------------------------------------------
	// getCurrentState - Public C_Engine
	// Description 
	//		Returns a pointer to the current state.
	//
	// Arguments:	None.
	// Returns:		a pointer to the current state.
	//-----------------------------------------------------------------------
	virtual EngineStatePtr getCurrentState() { return d_pCurrentState; }

	//-----------------------------------------------------------------------
	// getPreviousState - Public C_Engine
	// Description 
	//		Returns a pointer to the previous state.
	//
	// Arguments:	None.
	// Returns:		a pointer to the previous state.
	//-----------------------------------------------------------------------
	virtual EngineStatePtr getPreviousState() { return d_pPreviousState; }

	virtual void addSystem(EngineSystemPtr sys);
	virtual EngineSystemPtr getSystem(const SystemID& id);

	//-----------------------------------------------------------------------
	// isInState - Public C_Engine
	// Description 
	//		Returns true if the current state has the ID, id
	//
	// Arguments:	None.
	// Returns:		true if the engine is in "state", false otherwise
	//-----------------------------------------------------------------------
	virtual bool isInState(const StateID& id)
	{ return (id == d_pCurrentState->getID()); }

	void setCurrentState(EngineStatePtr curState);
	virtual void queueStateChange(EngineStatePtr newState);

	//-----------------------------------------------------------------------
	// setPreviousState - Public C_Engine
	// Description 
	//		Sets the previous state of the engine.
	//
	// Arguments:	prevState - the previous state for the engine.
	// Returns:		None.
	//-----------------------------------------------------------------------
	virtual void setPreviousState(EngineStatePtr prevState) 
	{ d_pPreviousState = prevState; }

	virtual MsgStatus queueMessage(MessagePtr msg);
	virtual MsgStatus triggerMessage(MessagePtr msg);
	virtual bool addWildCardListener(MsgListenerPtr l);
	virtual void deleteWildCardListener(MsgListenerPtr l);
	virtual bool addListener(MsgListenerPtr l, const MessageType& type);
	virtual bool deleteListener(MsgListenerPtr l, const MessageType& type);
};

#endif // _CENGINE_
