//======================================================================
// C_Keyboard.h 
// Author: James McCormick
// Discription:
//	A class for handling the keyboard
//======================================================================


#ifndef _KEYBOARD_
#define _KEYBOARD_

class I_KeyboardListener;

class C_Keyboard {
	unsigned char d_KeyState1[256];
	unsigned char d_KeyState2[256];
	unsigned char* d_pOldKeyState;
	unsigned char* d_pKeyState;
	unsigned char* d_pTempPtr;
 
	LPDIRECTINPUTDEVICE8 d_pDevice;

public:
	C_Keyboard() : d_pTarget(0), d_pDevice(0) {
		d_pOldKeyState = &d_KeyState1[0];
		d_pKeyState = &d_KeyState2[0];
	}

	~C_Keyboard();

	bool init(HWND hWnd, LPDIRECTINPUT8 pDevice);

	bool poll(int key);

	void clearTable();

	bool update();

	void processInput(KeyboardListenerPtr target);
};

#endif // _KEYBOARD_
