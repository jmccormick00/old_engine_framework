//======================================================================
// C_Mouse.h 
// Author: James McCormick
// Discription:
//	A class for handling the mouse input
//======================================================================


#ifndef _MOUSE_
#define _MOUSE_

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class C_Mouse {
	LPDIRECTINPUTDEVICE8	d_pDevice; 
	DIMOUSESTATE			d_lastState;
	DIMOUSESTATE			d_currState;

public:

	C_Mouse() : d_pDevice(0) {}
	~C_Mouse();

	bool init(HWND hWnd, LPDIRECTINPUT8 pDevice, bool bExclusive);

	bool update();

	bool acquire();
	void unAcquire();

	void processInput(MouseListenerPtr target);
};

#endif // _MOUSE_
