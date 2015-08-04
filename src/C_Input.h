//======================================================================
// C_Input.h 
// Author: James McCormick
// Discription:
//	The system that handles the input from the user
//======================================================================

#ifndef _CINPUT_
#define _CINPUT_

#define DIRECTINPUT_VERSION 0x0800

#include "I_InputInterface.h"
#include <dinput.h>

class C_Keyboard;
class C_Mouse;

class C_Input : public I_Input {
private:
	// The systems that handle each device
	C_Keyboard* d_pKeyboard;
	C_Mouse* d_pMouse;

	LPDIRECTINPUT8 d_pDirectInput;

	static C_Input d_inputLayerInstance;

	// Constructor/Destructor
	C_Input();
	C_Input(const C_Input&);

	// The special keyboard listener to send input to the GUI module
	class GUIKeyboardListener : public I_KeyboardListener {
	public:
		bool onKeyDown(int key);
		bool onKeyUp(int key);
	};
	// The special mouse listener to send input to the GUI module
	class GUIMouseListener : public I_MouseListener {
		void mouseMoved(long dx, long dy);
		void mouseButtonUp(MouseButton button);
		void mouseButtonDown(MouseButton button);
		void mouseWheel(short amt);
	};

	void processInput();

	KeyboardListenerPtr d_GUIKeyboardListener, d_keyboardListener;
	MouseListenerPtr d_GUIMouseListener, d_mouseListener;

public:

	~C_Input();

	void setFocus();
	void killFocus();
	void updateDevices();

	bool isShiftDown();
	bool isCtrlDown();
	bool isAltDown();

	void setMouseListener(MouseListenerPtr ml);
	void setKeyboardListener(KeyboardListenerPtr kl);

	// Use this to init the system
	bool init(const C_Window& window, bool bExclusive, MouseListenerPtr ml, KeyboardListenerPtr kl);
};

#endif // _CINPUT_
