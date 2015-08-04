

#ifndef _CWINDOW_
#define _CWINDOW_

#define WINDOWS_LEAN_AND_MEAN

// Let VC++ know we are compiling for WinXP and up.  This let's us use
// API functions specific to WinXP (e.g., WM_INPUT API).
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

#include <windows.h>
#include <string>

class C_Window {
private:
	bool d_bResizing;	// Is the resize bars being dragged
	bool d_bMinimized;
	bool d_bMaximized;

	HINSTANCE	d_hAppInstance;	// Application instance handle
	HWND		d_hMainWindow;	// The window handle

	MSG			d_msg;

	// The window's client dimensions
	unsigned d_iClientWidth;
	unsigned d_iClientHeight;

	// Window Title
	std::wstring d_MainWndCaption;

public:
	C_Window();
	~C_Window();

	bool msgPump();
	bool hasMessage();

	void setWindowCaption(const  std::wstring& cap) { d_MainWndCaption = cap; }
	void setClientWidth(unsigned width)				{ d_iClientWidth = width; }
	void setClientHeight(unsigned height)			{ d_iClientHeight = height; }
	MSG getCurrentMsg() { return d_msg; }

	HINSTANCE getAppHandle() const	{ return d_hAppInstance; }
	HWND getWindowHandle() const	{ return d_hMainWindow; }
	unsigned getWidth() const		{ return d_iClientWidth; }
	unsigned getHeight() const		{ return d_iClientHeight; }

	LRESULT msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool initWindow(HINSTANCE hInst, std::wstring windowName, unsigned windowWidth, unsigned windowHeight, bool fullScreen);
};

#endif // _CWINDOW_
