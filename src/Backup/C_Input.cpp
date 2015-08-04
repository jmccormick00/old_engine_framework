//======================================================================
// C_InputLayer.cpp
// Author: James McCormick
// Discription:
//	The system that handles the input from the user
//======================================================================

#include "C_Input.h"
#include "C_Window.h"
#include "C_Keyboard.h"
#include "C_Mouse.h"
#include "C_SystemLog.h"
#include "UtilityMacros.h"
#include "I_GuiInterface.h"

I_Input* G_Input;
C_Input C_Input::d_inputLayerInstance;

C_Input::C_Input() : d_pKeyboard(0), d_pMouse(0) {
	G_Input = this;
	d_GUIKeyboardListener = new GUIKeyboardListener();
	d_GUIMouseListener = new GUIMouseListener();
}

void C_Input::GUIMouseListener::mouseMoved(long dx, long dy) {
	G_GUI->onMouseMove(dx, dy);
}
		
void C_Input::GUIMouseListener::mouseButtonUp(MouseButton button) {
	G_GUI->onMouseButtonUp(button);
}

void C_Input::GUIMouseListener::mouseButtonDown(MouseButton button) {
	G_GUI->onMouseButtonDown(button);
}

bool C_Input::GUIKeyboardListener::onKeyDown(int key) {
	G_GUI->onKeyDown(key);
}

bool C_Input::GUIKeyboardListener::onKeyUp(int key) {
	G_GUI->onKeyUp(key);
}

C_Input::~C_Input() {
	if(d_pKeyboard)
		delete d_pKeyboard;
	if(d_pMouse)
		delete d_pMouse;
	ReleaseCOM(d_pDirectInput);
}

void C_Input::setFocus() {
	if(d_pKeyboard)
		d_pKeyboard->clearTable();
	if(d_pMouse)
		d_pMouse->acquire();
}

void C_Input::killFocus() {
	if(d_pKeyboard)
		d_pKeyboard->clearTable();
	if(d_pMouse)
		d_pMouse->unAcquire();
}

void C_Input::updateDevices() {
	if(d_pKeyboard)
		d_pKeyboard->update();
	if(d_pMouse)
		d_pMouse->update();

	processInput();
}


void C_Input::processInput() {
	if(G_GUI->hasInputFocus()) {
		d_pKeyboard->processInput(d_GUIKeyboardListener);
	} else { // Normal input processing
		d_pKeyboard->processInput(d_keyboardListener);
	}
}


bool C_Input::init(const C_Window& window, bool bExclusive, MouseListenerPtr mouseListener, KeyboardListenerPtr keyboardListener) {
	HRESULT hr;
	hr = DirectInput8Create(window.getAppHandle(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&d_pDirectInput, NULL);
	if( FAILED(hr) ) {
		DEBUGLOG(L"INPUT DIRECTINPUTCREATE FAILED.");
		return false;
	}

	// Create the keyboard
	d_pKeyboard = new C_Keyboard();
	if(d_pKeyboard == 0) {
		DEBUGLOG(L"INPUT KEYBOARD NEW FAILED.");
		return false;
	}

	if(!d_pKeyboard->init(window.getWindowHandle(), d_pDirectInput)) {
		DEBUGLOG(L"INPUT KEYBOARD INIT FAILED.");
		return false;
	}
	d_keyboardListener = keyboardListener;

	// Create the Mouse
	d_pMouse = new C_Mouse();
	if(d_pMouse == 0) {
		DEBUGLOG(L"INPUT MOUSE NEW FAILED.");
		return false;
	}

	if(!d_pMouse->init(window.getWindowHandle(), d_pDirectInput, bExclusive)) {
		DEBUGLOG(L"INPUT MOUSE INIT FAILED.");
		return false;
	}
	d_mouseListener = mouseListener;

	DEBUGLOG(L"INPUT INIT SUCCESSFUL.");
	return true;
}
