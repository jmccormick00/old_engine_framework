//======================================================================
// C_Keyboard.cpp 
// Author: James McCormick
// Discription:
//	A class for handling the keyboard
//======================================================================
#define DIRECTINPUT_VERSION 0x0800
#include "UtilityMacros.h"
#include "C_SystemLog.h"
#include "I_InputInterface.h"
#include <memory.h>
#include <dinput.h>
#include "C_Keyboard.h"

bool C_Keyboard::init(HWND hWnd, LPDIRECTINPUT8 lpdi)	{
	HRESULT hr;

	// Get a keyboard device
	hr = lpdi->CreateDevice( GUID_SysKeyboard, &d_pDevice, NULL );
	if( FAILED(hr) ) {
		DEBUGLOG(L"KEYBOARD Device FAILED.");
		d_pDevice = NULL;
		return false;
	}

	// Set the keyboard data format
	hr = d_pDevice->SetDataFormat(&c_dfDIKeyboard);
	if( FAILED(hr) ) {
		ReleaseCOM(d_pDevice);
		DEBUGLOG(L"KEYBOARD SetDataFormat FAILED.");
		return false;
	}

	// Set Cooperative level
	hr = d_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	if( FAILED(hr) ) {
		ReleaseCOM(d_pDevice);
		DEBUGLOG(L"KEYBOARD SetCooperativeLevel FAILED.");
		return false;
	}

	clearTable();
	DEBUGLOG(L"KEYBOARD INIT SUCCESSFUL.");
	return true;
}

C_Keyboard::~C_Keyboard() {
	if(d_pDevice) {
		d_pDevice->Unacquire();
		ReleaseCOM(d_pDevice);
	}
}

bool C_Keyboard::poll( int key ) {
	if( d_pKeyState[key] & 0x80 )
		return true;
	return false;
}

void C_Keyboard::clearTable() {
	memset(d_pKeyState, 0, sizeof(unsigned char)*256 );
}

bool C_Keyboard::update() {
    HRESULT  hr; 
 
	// Swap Pointers
	d_pTempPtr = d_pOldKeyState;
	d_pOldKeyState = d_pKeyState;
	d_pKeyState = d_pTempPtr;

    hr = d_pDevice->Poll(); 
    hr = d_pDevice->GetDeviceState(256,(LPVOID)d_pKeyState); 

    if( FAILED(hr) ) { 
		hr = d_pDevice->Acquire();
		if( FAILED( hr ) )
			return false;

	    hr = d_pDevice->Poll(); 
	    hr = d_pDevice->GetDeviceState(256,(LPVOID)d_pKeyState); 
		if( FAILED( hr ) )
			return false;
    } 
 
	return true;
}

void C_Keyboard::processInput(KeyboardListenerPtr target) {
	if(target) {
		for(int i = 0; i < 256; i++) {
			if( d_pOldKeyState[i] != d_pKeyState[i] ) {
				// Something happened to this key since the last time we checked
				if( !(d_pKeyState[i] & 0x80) ) {
					// It was Released
					target->onKeyUp(i);
				} else {
					// Do nothing; it was just pressed, it'll get a keydown 
					// in a bit, and we dont' want to send the signal to the 
					// input target twice
				}
			}

			// It was pressed
			if(poll(i)) target->onKeyDown(i);
		}
	}
}
