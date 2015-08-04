/*=========================================================================
/ James McCormick - C_Engine.cpp
/ A class for handling the objects and storing the scene
/==========================================================================*/

#include "C_Engine.h"
#include "I_SceneInterface.h"
#include "I_GuiInterface.h"
#include "C_Console.h"
#include <algorithm>

//========================================================================
// C_Engine implemenation
//========================================================================
I_Engine* G_Engine;
C_Engine C_Engine::d_Engine;

//-----------------------------------------------------------------------
// Constructor - Public C_Engine
// Description 
//	Initializiations.
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
C_Engine::C_Engine() : d_NewObjectID(0), d_bIsPaused(false), d_bCurrentMsgQueue(false) {
	d_vUpdateList.reserve(MAXSYSTEMS);
	d_vRenderList.reserve(MAXSYSTEMS); 
	G_Engine = this;
}


//-----------------------------------------------------------------------
// destructor - Public C_Engine
// Description 
//	
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
C_Engine::~C_Engine() {
	if(d_pPreviousState)
		d_pPreviousState = 0;

	if(d_pCurrentState) {
		d_pCurrentState->exit();
		d_pCurrentState = 0;
	}

	clearSystems();
	clearObjects();
	d_ListenerMap.clear();
}

//-----------------------------------------------------------------------
// clearSystems - Public C_Engine
// Description 
//		Clears the list of systems
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::clearSystems() {
	clearSystemLists();
	d_SystemMap.clear();
}

//-----------------------------------------------------------------------
// clearSystemLists - Public C_Engine
// Description 
//		Clears the list of systems
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::clearSystemLists() {
	d_vUpdateList.clear();		
	d_vRenderList.clear();
}


//-----------------------------------------------------------------------
// addSystem - Public C_Engine
// Description 
//		Adds a system to the engine.
//
// Arguments:	
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::addSystem(EngineSystemPtr sys) {
	SystemID id = sys->getID();
	d_SystemMap[id] = sys;
}


//-----------------------------------------------------------------------
// getSystem - Public C_Engine
// Description 
//		Returns a pointer to a system with the id.  Returns null if
//		the system doesn't exist
//
// Arguments:	id - the id of the systme to get	
// Returns:		None.
//-----------------------------------------------------------------------
EngineSystemPtr C_Engine::getSystem(const SystemID& id) {
	SystemMap::iterator i = d_SystemMap.find(id);
	if(i == d_SystemMap.end()) {
		EngineSystemPtr null;
		return null;
	}

	return (*i).second;
}

//-----------------------------------------------------------------------
// addObject - Public C_Engine
// Description 
//		Takes a pointer to a new object, assigns it an id and adds it
//		to the object map.
//
// Arguments:	object - the pointer to the new object
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::addObject(ObjectPtr object) {
	ObjectID id = d_NewObjectID;
	d_ObjectMap[id] = object;
	object->d_ID = id;
	++d_NewObjectID;
}


//-----------------------------------------------------------------------
// deleteObject - Public C_Engine
// Description 
//		Takes an object id and removes the associated object 
//		from the map and deletes it.
//
// Arguments:	id - the id of the object to be deleted
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::deleteObject(const ObjectID& id) {
	SystemMap::iterator itr = d_SystemMap.begin();
	SystemMap::iterator endItr = d_SystemMap.end();
	for(; itr != endItr; ++itr)
		itr->second->onObjectDeletion(id);
	
	//std::for_each(d_SystemMap.begin(), d_SystemMap.end(), std::mem_fun(&I_EngineSystem::onObjectDeletion));

	// TODO - tell the scene to remove the object

	d_ObjectMap.erase(id);
}


//-----------------------------------------------------------------------
// getObject - Public C_Engine
// Description 
//		Takes the ObjectID of an object and returns a pointer to the
//		associated object.  Null if the object wasnt found.
//
// Arguments:	id - the id of the object to get.
// Returns:		A pointer to the object.
//-----------------------------------------------------------------------
ObjectPtr C_Engine::getObject(const ObjectID& id) {
	ObjectMap::iterator i = d_ObjectMap.find(id);
	if(i == d_ObjectMap.end()) {
		ObjectPtr null;
		return null;
	}
	
	return (*i).second;
}


//-----------------------------------------------------------------------
// onUpdate - Private C_Engine
// Description 
//		Steps the engine and the subsystems by the delta t.
//
// Arguments:	elapsedTime - the delta time from the last update.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::onUpdate(const double& elapsedTime) {
	if(!d_bIsPaused) {
		if(!d_vUpdateList.empty()) {
			SystemList::iterator itr = d_vUpdateList.begin();
			SystemList::iterator end = d_vUpdateList.end();
			for(;itr != end; ++itr)
				if(!(*itr)->isPaused())
					(*itr)->onUpdate(elapsedTime);
		}
	}
}


//-----------------------------------------------------------------------
// onPreRender - Private C_Engine
// Description 
//		Calls all the objects OnPreRender function so they can prepare
//		to draw.
//
// Arguments:	elapsedTime - the delta time from the last update.
// Returns:		None.
//-----------------------------------------------------------------------
//void C_Engine::onPreRender(const double& elapsedTime) {
//	if(!d_vRenderList.empty()) {
//		SystemList::iterator itr = d_vRenderList.begin();
//		SystemList::iterator end = d_vRenderList.end();
//		for(;itr != end; ++itr)
//			if((*itr)->isVisible())
//				(*itr)->onPreRender(elapsedTime);
//	}
//}


//-----------------------------------------------------------------------
// setCurrentState - Public C_Engine
// Description 
//		Changes the state of of the engine.  Saves the previous state.
//
// Arguments:	newState - the new state for the engine.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::setCurrentState(EngineStatePtr curState) {
	if(d_pCurrentState) {
		d_pCurrentState->exit();
		d_pPreviousState = d_pCurrentState;
	}
	d_pCurrentState = curState;
	d_pCurrentState->enter();
}


//-----------------------------------------------------------------------
// queueStateChange - Public C_Engine
// Description 
//		Sets the new state of the engine.  Saves the previous state.
//
// Arguments:	newState - the new state for the engine.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::queueStateChange(EngineStatePtr newState) {
	d_pQueuedState = newState;
}


//-----------------------------------------------------------------------
// revertToPreviousState - Public C_Engine
// Description 
//		Reverts the engine to the previous state.
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::revertToPreviousState() {
	if(d_pPreviousState)
		d_pQueuedState = d_pPreviousState;
}


//-----------------------------------------------------------------------
// deleteSystem - Public C_Engine
// Description 
//		Removes the system with the ID id.
//
// Arguments:	id - The id of the system to delete.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::deleteSystem(const SystemID& id) {
	SystemMap::iterator itr = d_SystemMap.find(id);
	if(itr != d_SystemMap.end()) {
		if(!d_vRenderList.empty())
			d_vRenderList.erase(std::remove(d_vRenderList.begin(), d_vRenderList.end(), itr->second));
		if(!d_vUpdateList.empty())
			d_vUpdateList.erase(std::remove(d_vUpdateList.begin(), d_vUpdateList.end(), itr->second));
		d_SystemMap.erase(id);
	}
}


//-----------------------------------------------------------------------
// onRender - Private C_Engine
// Description 
//		Loops thru the render list and calls render.
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::onRender() {
	if(!d_vRenderList.empty()) {
		SystemList::iterator itr = d_vRenderList.begin();
		SystemList::iterator end = d_vRenderList.end();
		for(;itr != end; ++itr)
			if((*itr)->isVisible())
				(*itr)->onRender();
	}
}


//-----------------------------------------------------------------------
// addListener - Public C_Engine
// Description 
//		Pairs a message type to a message listener.
//
// Arguments:	MsgListenerPtr - A pointer to a message listener
//				MessageType - the message type to pair the listener to.
// Returns:		true if successful, false otherwise
//-----------------------------------------------------------------------
bool C_Engine::addListener(MsgListenerPtr l, const MessageType& type) {
	// Check to see if one exists first
	MessageListenerMap::iterator listenerMapItr = d_ListenerMap.find(type);
	if(listenerMapItr == d_ListenerMap.end()) {
		MessageListenerMapIResult insertResult = d_ListenerMap.insert(
			MessageListenerMapPair(type, ListenerList()));

		// Pair insertion failed!!
		if(!insertResult.second)
			return false;

		// Somehow created an empty table?
		if(insertResult.first == d_ListenerMap.end())
			return false;
		
		// replace the listenerList iterator so it can be updated
		listenerMapItr = insertResult.first;
	}


	// Loop through the list to make sure no duplicates exist
	ListenerList & lList = (*listenerMapItr).second;
	for(ListenerList::iterator it = lList.begin(), itEnd = lList.end(); it != itEnd; it++) {
		if(*it == l) 
			return false;
	}

	// Everything checks out, add the listener
	lList.push_back(l);
	return true;
}


//-----------------------------------------------------------------------
// addWildCardListener - Public C_Engine
// Description 
//		Adds a listener as a wild card.
//
// Arguments:	MsgListenerPtr - A pointer to a message listener
// Returns:		true if successful, false otherwise
//-----------------------------------------------------------------------
bool C_Engine::addWildCardListener(MsgListenerPtr l) {
	// First check to make sure this doesnt already exist
	//ListenerList::iterator i = (l);

	// This listener is already in the list
	//if(i == d_WildCardListenerList.end())
	//	return false;

	// Add it to the list.
	d_WildCardListenerList.push_back(l);
	
	return true;
}




//-----------------------------------------------------------------------
// triggerMessage - Public C_Engine
// Description 
//		Processes the message now.
//
// Arguments:	MessagePtr - a pointer to the message to send now.
// Returns:		an enum value:	E_NOTCONSUMED - the msg was not consumed
//								E_CONSUMED - msg was consumed,
//								E_NOLISTENER - no listener for msg
//-----------------------------------------------------------------------
C_Engine::MsgStatus C_Engine::triggerMessage(MessagePtr msg) {
	C_Engine::MsgStatus status = E_NOTCONSUMED;
	ListenerList::iterator listItr;
	ListenerList::iterator listItrEnd; 

	MessageListenerMap::iterator mapItr = d_ListenerMap.find(msg->getType());
	if(mapItr == d_ListenerMap.end()) 
		status = E_NOLISTENER;
	else {
		listItr = mapItr->second.begin();
		listItrEnd = mapItr->second.end();
		for(listItr; listItr != listItrEnd; ++listItr) {
			if((*listItr)->onMessage(msg))
				status = E_CONSUMED;
		}
	}

	// Send the message to the wildcard listeners
	listItr = d_WildCardListenerList.begin();
	listItrEnd = d_WildCardListenerList.end();
	for(; listItr != listItrEnd; ++listItr)
		if((*listItr)->onMessage(msg))
			status = E_CONSUMED;

	return status;
}


//-----------------------------------------------------------------------
// queueMessage - Public C_Engine
// Description 
//		Queue the message to be processed on the next iteration.
//
// Arguments:	MessagePtr - a pointer to the message to queue.
// Returns:		an enum value:	E_NOTCONSUMED - the msg was not consumed
//								E_NOLISTENER - no listener for msg
//								E_SUCCESS - success
//-----------------------------------------------------------------------
C_Engine::MsgStatus C_Engine::queueMessage(MessagePtr msg) {
	// Check for a listener, if no listeners then skip the msg.
	MessageListenerMap::iterator mapItr = d_ListenerMap.find(msg->getType());
	if(mapItr == d_ListenerMap.end())
		return E_NOLISTENER;

	if(mapItr->second.empty())
		return E_NOLISTENER;

	d_MessageQueue[d_bCurrentMsgQueue].push_back(msg);
	return E_SUCCESS;
}


//-----------------------------------------------------------------------
// deleteListener - Public C_Engine
// Description 
//		Removes a listener and message type pairing.
//
// Arguments:	MsgListenerPtr - the pointer to the message listener.
//				MessageType - the message type
// Returns:		true if sucessful, false if the listener wasn't found
//-----------------------------------------------------------------------
bool C_Engine::deleteListener(MsgListenerPtr l, const MessageType& type) {
	MessageListenerMap::iterator mapItr = d_ListenerMap.find(type);
	if(mapItr != d_ListenerMap.end()) {
		mapItr->second.remove(l);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------
// deleteWildCardListener - Public C_Engine
// Description 
//		Removes a Wild Card listener.
//
// Arguments:	MsgListenerPtr - the pointer to the message listener.
// Returns:		none.
//-----------------------------------------------------------------------
void C_Engine::deleteWildCardListener(MsgListenerPtr l) {
		d_WildCardListenerList.remove(l);
}

//-----------------------------------------------------------------------
// dispatchMessages - Private C_Engine
// Description 
//		Send all of the queued messages.
//
// Arguments:	None.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::dispatchMessages() {
	// Switch the current queue so that during message processing more msgs can be sent.
	bool queueToProcess = d_bCurrentMsgQueue;
	d_bCurrentMsgQueue = !d_bCurrentMsgQueue;
	d_MessageQueue[d_bCurrentMsgQueue].clear();

	// Dispatch messages  
	while(!d_MessageQueue[queueToProcess].empty()) {
		MessagePtr msg = d_MessageQueue[queueToProcess].front();
		d_MessageQueue[queueToProcess].pop_front();
		if(triggerMessage(msg) == E_NOTCONSUMED)
			d_MessageQueue[d_bCurrentMsgQueue].push_back(msg);		
	}
}


//-----------------------------------------------------------------------
// tick - Public C_Engine
// Description 
//		Steps the engine in time.  Calls DispatchMessages, OnUpdate,
//		and OnRender.
//
// Arguments:	none.
// Returns:		None.
//-----------------------------------------------------------------------
void C_Engine::tick() {
	static double elapsedTime;
	// First - Update the timer 
	d_timer.tick();
	d_dCurTimeStamp = d_timer.getTime(); 
	elapsedTime = d_timer.getDeltaTime();

	// Process the Console
	G_Console->onUpdate(elapsedTime);

	// Second - Send out the messages
	dispatchMessages();

	// Third - Check if the engine state needs updated
	if(d_pQueuedState) {
		setCurrentState(d_pQueuedState);
		d_pQueuedState = 0;
	}

	// Fourth - Perform the updates
	onUpdate(elapsedTime);
	if(G_SceneManager->hasScene())
		G_SceneManager->getCurrentScene()->onUpdate(elapsedTime);
	//G_GUI->onUpdate(elapsedTime);

	// Fifth - Perform the rendering to the screen
	//onPreRender(elapsedTime);			// TODO - do i need a pre render function?
	onRender();
	if(G_SceneManager->hasScene())
		G_SceneManager->getCurrentScene()->onRender();
	//G_GUI->onRender();
	if(G_Console->isActive())
		G_Console->onRender();
}
