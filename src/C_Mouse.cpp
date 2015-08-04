//======================================================================
// C_Mouse.h 
// Author: James McCormick
// Discription:
//	A class for handling the mouse input
//======================================================================
#define DIRECTINPUT_VERSION 0x0800
#include "C_SystemLog.h"
#include "I_InputInterface.h"
#include "UtilityMacros.h"
#include <dinput.h>
#include "C_Mouse.h"

void C_Mouse::processInput(MouseListenerPtr target) {
	if( target ) {
		int dx = d_currState.lX;
		int dy = d_currState.lY;
		if( dx || dy ) target->mouseMoved(dx, dy);
		if( d_currState.lZ != 0 ) {
			// Mouse Wheel was moved
			target->mouseWheel(d_currState.lZ);
		}
		if( d_currState.rgbButtons[0] & 0x80 ) {
			// the button got pressed.
			target->mouseButtonDown(0);
		}
		if( d_currState.rgbButtons[1] & 0x80 ) {
			// the button got pressed.
			target->mouseButtonDown(1);
		}
		if( d_currState.rgbButtons[2] & 0x80 ) {
			// the button got pressed.
			target->mouseButtonDown( 2 );
		}
		if( !(d_currState.rgbButtons[0] & 0x80) && (d_lastState.rgbButtons[0] & 0x80) ) {
			// the button got released.
			target->mouseButtonUp( 0 );
		}
		if( !(d_currState.rgbButtons[1] & 0x80) && (d_lastState.rgbButtons[1] & 0x80) ) {
			// the button got released.
			target->mouseButtonUp( 1 );
		}
		if( !(d_currState.rgbButtons[2] & 0x80) && (d_lastState.rgbButtons[2] & 0x80) ) {
			// the button got released.
			target->mouseButtonUp( 2 );
		}
	}
	d_lastState = d_currState;
}


bool C_Mouse::init(HWND hWnd, LPDIRECTINPUT8 pDevice, bool bExclusive) {
	HRESULT hr;

	// Get a Mouse device
	hr = pDevice->CreateDevice(GUID_SysMouse, &d_pDevice, NULL);
	if( FAILED(hr) ) {
		DEBUGLOG(L"MOUSE Device FAILED.");
		return false;
	}

	hr = d_pDevice->SetDataFormat(&c_dfDIMouse);
	if( FAILED(hr) ) {
		ReleaseCOM(d_pDevice);
		DEBUGLOG(L"MOUSE SetDataFormat FAILED.");
		return false;
	}

	if(bExclusive)
		hr = d_pDevice->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	else
		hr = d_pDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	if( FAILED(hr) ) {
		ReleaseCOM(d_pDevice);
		DEBUGLOG(L"MOUSE SetCooperativeLevel FAILED.");
		return false;
	}

	G_SystemLog->post(L"MOUSE Init SUCCESSFUL.");

	d_lastState.lX = 0;
	d_lastState.lY = 0;
	d_lastState.lZ = 0;
	d_lastState.rgbButtons[0] = 0;
	d_lastState.rgbButtons[1] = 0;
	d_lastState.rgbButtons[2] = 0;
	d_lastState.rgbButtons[3] = 0;
	return true;
}

C_Mouse::~C_Mouse() {
	if(d_pDevice) {
		d_pDevice->Unacquire();
		ReleaseCOM(d_pDevice);
	}
}

bool C_Mouse::update() {
    HRESULT  hr; 
 
    hr = d_pDevice->Poll(); 
	hr = d_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&d_currState);

    if( FAILED(hr) ) { 
		hr = d_pDevice->Acquire();
		if( FAILED( hr ) ) {
			return false;
		}

	    hr = d_pDevice->Poll(); 
	    hr = d_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&d_currState); 
		if( FAILED( hr ) ) {
			return false;
		}
    } 
	return true;
}

bool C_Mouse::acquire() {
	HRESULT hr = d_pDevice->Acquire();
	if( FAILED(hr) )
		return false;
	return false;
}

void C_Mouse::unAcquire() {
	d_pDevice->Unacquire();
}
