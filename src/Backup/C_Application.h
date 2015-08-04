//======================================================================
// C_Application.h 
// Author: James McCormick
// Discription:
//	The implementation of the application module
//======================================================================

#ifndef _CAPPLICATION_
#define _CAPPLICATION_

#include "I_AppInterface.h"
#include "C_Window.h"

class C_Application : public I_Application {
private:
	C_Window d_window;
	AppInstallerPtr d_installer;

	C_Application() {
		G_Application = this;
	}
	C_Application(const C_Application&);
	static C_Application d_instance;

	KeyboardListenerPtr d_pKeyboardTarget;
	MouseListenerPtr d_pMouseTarget;

public:
	C_Window* getWindow() { return &d_window; }

	int run();
	void init(HINSTANCE hinst);
	void setAppInstaller(AppInstallerPtr a) { d_installer = a; }
	void onResize();
	void onKillFocus();
	void onSetFocus();
	void setMouseListener(MouseListenerPtr m) { d_pMouseTarget = m; }
	void setKeyboardListener(KeyboardListenerPtr k) { d_pKeyboardTarget = k;}
	// The message proc
	bool msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif //_CAPPLICATION_
