//======================================================================
// I_GuiInterface.h 
// Author: James McCormick
// Discription:
//	The interfaces for the GUI module
//======================================================================

#ifndef _IGUIINTERFACE_
#define _IGUIINTERFACE_

#include "C_SmartPointer.h"
#include "3DMath.h"

//-----------------------------------------------------------------------
// The basic element in the gui
// For a gui element, a point is described by a 3D vector.  The x and y
// values store the position on the screen.  The z value stores the depth
// To send model updates to an element make the I_GUIElement a message listener
// as well and send messesges with the updated info.
class I_GUIElement : public C_SmartPointerObject {
public:
	virtual void onRestore() = 0;
	virtual void onRender() = 0;
	virtual void onUpdate(const double& elapsedTime) = 0;

	virtual float getZOrder() const = 0;
	virtual void setZOrder(const float z) = 0;
	virtual bool isVisible() const = 0;
	virtual void setVisible(bool visible) = 0;
	virtual void getBounds(Math::vector3& max, Math::vector3& min) = 0;
	virtual ~I_GUIElement() { };
};
typedef C_SmartPointer<I_GUIElement> GUIElementPtr;

// The implementation for the application specific GUI logic and layout
class I_GUILogic : public C_SmartPointerObject {
public:
	virtual void onKeyPress(int key) = 0;
	virtual void setUpGUI() = 0;			// Called to set up the layout of the curent interface
	virtual void onUpdate(const double& elapsedTime) = 0;
	virtual void clean() = 0;				// Called when this logic is replaced
};
typedef C_SmartPointer<I_GUILogic> GUILogicPtr;

class I_GUI {
public:
	virtual bool hasInputFocus() = 0;
	virtual void onMouseButtonDown(short button) = 0;
	virtual void onMouseButtonUp(short button) = 0;
	virtual void onMouseMove(long dx, long dy) = 0;
	virtual void onMouseWheel(short amt) = 0;
	virtual void onKeyDown(int key) = 0;
	virtual void onKeyUp(int key) = 0;
	virtual void onRender() = 0;
	virtual void onUpdate(const double& elapsedTime) = 0;
	virtual void setCurrentLogic(GUILogicPtr guiLogic) = 0;
	virtual void onRestore() = 0;
};

extern I_GUI* G_GUI;

#endif // _IGUIINTERFACE_
