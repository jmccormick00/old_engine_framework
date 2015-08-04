
#include "C_SystemLog.h"
#include "I_EngineInterface.h"
#include "C_Application.h"
#include "C_Graphics.h"
#include "C_Window.h"

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//static bool first = true;
	//if(msg == WM_CREATE) first = false; // Don't push the msg's to the application until it is created.
	//if(first) return DefWindowProc(hwnd, msg, wParam, lParam);
	//else return G_Application->getWindow()->msgProc(hwnd, msg, wParam, lParam);
	static C_Application* app = (C_Application*)G_Application;
	if(app->msgProc(hwnd, msg, wParam, lParam))
		return app->getWindow()->msgProc(hwnd, msg, wParam, lParam);
	return 0;
}


C_Window::C_Window() : d_iClientWidth(0), d_iClientHeight(0),
d_hAppInstance(0), d_hMainWindow(0), d_MainWndCaption(L""),
d_bResizing(false), d_bMinimized(false), d_bMaximized(false)
{}

C_Window::~C_Window()
{}

bool C_Window::hasMessage() {
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		return true;
	return false;
}


bool C_Window::msgPump() {
	MSG msg = {0};
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	if(WM_QUIT == msg.message)
		return false;
	TranslateMessage( &msg );
	DispatchMessage( &msg );
	return true;
}

bool C_Window::initWindow(HINSTANCE hInst, std::wstring windowName, unsigned windowWidth, unsigned windowHeight, bool fullScreen) { // TODO - fix the fullscreen code
	d_hAppInstance = hInst;
	d_iClientWidth = windowWidth;
	d_iClientHeight = windowHeight;
	d_MainWndCaption = windowName;

	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof(WNDCLASSEX) );

	// fill in some default values
	wc.cbSize = sizeof(WNDCLASSEX); 
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= 0;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= L"D3DWndClassName";
	wc.hIconSm			= 0;

	/*wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";
	wc.hIconSm		 = LoadIcon(0, IDI_APPLICATION);*/

	if( !RegisterClassEx(&wc) ) {
		DEBUGLOG(L"RegisterClass FAILED.");
		//MessageBox(0, L"RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions
	RECT R = { 0, 0, d_iClientWidth, d_iClientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	d_hMainWindow = CreateWindow(
						  L"D3DWndClassName",    // name of the window class
                          d_MainWndCaption.c_str(),   // title of the window
                          WS_OVERLAPPEDWINDOW | WS_VISIBLE,    // window style
                          CW_USEDEFAULT,    // x-position of the window
                          CW_USEDEFAULT,    // y-position of the window
                          width,    // width of the window
                          height,    // height of the window
                          NULL,    // we have no parent window, NULL
                          NULL,    // we aren't using menus, NULL
                          d_hAppInstance,    // application handle
                          NULL);    // used with multiple windows, NULL

	//DWORD dw = GetLastError(); 
	if( !d_hMainWindow ) {
		DEBUGLOG(L"Window Creation FAILED.");
		//MessageBox(0, L"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
		return false;
	} else {
		DEBUGLOG(L"Window Creation SUCCESSFUL.");
		ShowWindow(d_hMainWindow, SW_SHOW);
		UpdateWindow(d_hMainWindow);
		return true;
	}
}

LRESULT C_Window::msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
		case WM_ACTIVATE: {
			if( LOWORD(wParam) == WA_INACTIVE )
				G_Engine->pause();								// TODO - does the application need to paused so it can go into a paused state
			else
				G_Engine->unPause();
			return 0;
		}
		// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE: {
			// Save the new client area dimensions
			setClientWidth(LOWORD(lParam));
			setClientHeight(HIWORD(lParam));
			if(G_Graphics->getDevice()) {
				if(wParam == SIZE_MINIMIZED) {
					G_Application->onKillFocus();
					d_bMinimized = true;
					d_bMaximized = false;
				}
				else if(wParam == SIZE_MAXIMIZED) {
					d_bMinimized = false;
					d_bMaximized = true;
					G_Application->onResize();
					//G_Engine->UnPauseEngine();
				}
				else if(wParam == SIZE_RESTORED) {
					// Restoring from minimized state?
					if(d_bMinimized) {
						d_bMinimized = false;
						G_Application->onSetFocus();
					}

					// Restoring from maximized state?
					else if(d_bMaximized) {
						d_bMaximized = false;
						G_Application->onResize();
					}
					else if(d_bResizing) {
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					} else { // API call such as SetWindowPos or d_SwapChain->SetFullscreenState.
						G_Application->onResize();
					}
				}
			}
			return 0;
		}

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE: {
			G_Engine->pause();
			d_bResizing  = true;
			return 0;
		}

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE: {
			d_bResizing  = false;
			G_Application->onResize();
			G_Engine->unPause();
			return 0;
		}
		// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO: {
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 500; 
			return 0;
		}
        case WM_CLOSE: {
            DestroyWindow(d_hMainWindow);
			break;
		}
        case WM_DESTROY: {
            PostQuitMessage(0);
			break;
		}
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
