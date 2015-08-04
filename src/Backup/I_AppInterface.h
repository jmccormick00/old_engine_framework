//======================================================================
// I_AppInterface.h 
// Author: James McCormick
// Discription:
//	The interfaces for the application module
//======================================================================
#ifndef _APPINTERFACE_
#define _APPINTERFACE_

#include "C_SmartPointer.h"
#include <string>

class C_Window;

enum MouseButton {
	E_LEFT,
	E_MIDDLE,
	E_RIGHT,
	E_SIDE1,
	E_SIDE2
};

class I_KeyboardListener : public C_SmartPointerObject {
public:
	virtual void onKeyDown(const unsigned char key) = 0;
	virtual void onKeyUp(const unsigned char key) = 0;
};
typedef C_SmartPointer<I_KeyboardListener> KeyboardListenerPtr;

class I_MouseListener : public C_SmartPointerObject {
public:
	virtual void onMove( const short x, const short y ) = 0;
	virtual void onButtonUp( const MouseButton b, const short x, const short y ) = 0;
	virtual void onButtonDown( const MouseButton b, const short x, const short y ) = 0;
	// Negative values for downward movements and positive for upward movements
	virtual void onWheel(const short amt, const short x, const short y) = 0;
};
typedef C_SmartPointer<I_MouseListener> MouseListenerPtr;


// The object that is responsible for setting up the framework for the specific application 
class I_AppInstaller : public C_SmartPointerObject {
public:
	~I_AppInstaller() {}
	virtual void setUpFramework() = 0;
	virtual bool isFullScreen() = 0;
	virtual unsigned getWindowHeight() = 0;
	virtual unsigned getWindowWidth() = 0;
	virtual const std::wstring& getAppTitle() = 0;
	virtual MouseListenerPtr getMoustListener() = 0;
	virtual KeyboardListenerPtr getKeyboardListener() = 0;
};
typedef C_SmartPointer<I_AppInstaller> AppInstallerPtr;

//-----------------------------------------------------------------------
// The interface to the Application layer.  Subsystems use this
// interface to interact with the application module
class I_Application {
public:
	~I_Application() {}

	virtual C_Window* getWindow() = 0;

	virtual void setAppInstaller(AppInstallerPtr a) = 0;
	virtual void onResize() = 0;
	virtual void onKillFocus() = 0;
	virtual void onSetFocus() = 0;
	virtual void setMouseListener(MouseListenerPtr m) = 0;
	virtual void setKeyboardListener(KeyboardListenerPtr k) = 0;
};

extern I_Application* G_Application;

#endif // _APPINTERFACE_
