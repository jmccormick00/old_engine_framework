//======================================================================
// C_Application.cpp 
// Author: James McCormick
// Discription:
//	The implementation of the application module
//======================================================================

#include "C_Application.h"
#include "C_Graphics.h"
#include "C_Engine.h"
#include "C_Window.h"
#include "C_Console.h"

I_Application* G_Application;
C_Application C_Application::d_instance;

void C_Application::init(HINSTANCE hinst) {
	d_window.initWindow(hinst, 
						d_installer->getAppTitle(), 
						d_installer->getWindowWidth(), 
						d_installer->getWindowHeight(), 
						d_installer->isFullScreen());
	G_Graphics->initDirect3D(d_window);
	setMouseListener(d_installer->getMoustListener()); 
	setKeyboardListener(d_installer->getKeyboardListener());
	d_installer->setUpFramework();
}

void C_Application::onKillFocus() {
	G_Engine->pause();
}

void C_Application::onResize() {
	G_Graphics->onResize(d_window);
}

void C_Application::onSetFocus() {
	G_Engine->unPause();
}

int C_Application::run() {
	((C_Engine*)G_Engine)->start(); // Start the engine
	while(d_window.msgPump()) {
		G_Graphics->clearBackBuffer();	// Clear the back buffer
		G_Engine->tick();				// Step the engine one frame
		G_Graphics->presentFrame();		// Swap the back buffer to the front
	}
	G_Engine->clearObjects();
	G_Engine->clearSystems();
	return (int)(d_window.getCurrentMsg()).wParam;
}

bool C_Application::msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
	case WM_KEYDOWN:
		if(G_Console->isActive()) {
			// Let the console eat this.
		} else if(d_pKeyboardTarget) {
			d_pKeyboardTarget->onKeyDown(static_cast<const BYTE>(wParam));
		}
		break;
	case WM_KEYUP:
		if(G_Console->isActive()) {
			// let the console eat this
		} else if(d_pKeyboardTarget) {
			d_pKeyboardTarget->onKeyUp(static_cast<const BYTE>(wParam));
		}
		break;
	case WM_MOUSEMOVE:
		if(d_pMouseTarget) {
			d_pMouseTarget->onMove(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_LBUTTONDOWN:
		if(d_pMouseTarget) {
			d_pMouseTarget->onButtonDown(E_LEFT, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_LBUTTONUP:
		if(d_pMouseTarget) {
			d_pMouseTarget->onButtonUp(E_LEFT, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_RBUTTONDOWN:
		if(d_pMouseTarget) {
			d_pMouseTarget->onButtonDown(E_RIGHT, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_RBUTTONUP:
		if(d_pMouseTarget) {
			d_pMouseTarget->onButtonUp(E_RIGHT, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_MBUTTONDOWN:
		if(d_pMouseTarget) {
			d_pMouseTarget->onButtonDown(E_MIDDLE, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_MBUTTONUP:
		if(d_pMouseTarget) {
			d_pMouseTarget->onButtonUp(E_MIDDLE, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_XBUTTONDOWN:
		if(d_pMouseTarget) { 
			if(HIWORD(wParam) == XBUTTON1)
				d_pMouseTarget->onButtonDown(E_SIDE1, LOWORD(lParam), HIWORD(lParam));
			else
				d_pMouseTarget->onButtonDown(E_SIDE2, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_XBUTTONUP:
		if(d_pMouseTarget) {
			if(HIWORD(wParam) == XBUTTON1)
				d_pMouseTarget->onButtonDown(E_SIDE1, LOWORD(lParam), HIWORD(lParam));
			else
				d_pMouseTarget->onButtonDown(E_SIDE2, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_CHAR:
		if(G_Console->isActive()) {
			if(('~' == wParam) || ('`' == wParam)) {
				G_Console->close();
			} else {
				switch(wParam) {
				case 0x08:	// Backspace
					G_Console->onBackSpace();
					break;
				case 0x09:	// tab
					G_Console->onTab();
					break;
				case 0x0D:	// enter
					G_Console->onEnter();
					break;
				case 0x20:	// spacebar
					G_Console->onSpaceBar();
					break;
				default:
					G_Console->onKeyboardInput((char)wParam);
					break;
				}
			}
		} else {
			if(('~' == wParam) || ('`' == wParam))
				G_Console->open();
		}
		break;
	default:
		return false;
	}
	return true;
}
