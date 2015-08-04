/*=========================================================================
/ James McCormick - C_Graphics.h
/ A class for managing the DirectX layer
/==========================================================================*/

#ifndef _CGRAPHICS_
#define _CGRAPHICS_

#include <d3dx10.h>
#include <string>

class C_Window;

class C_Graphics {
private:
	// Direct3D variables
	ID3D10Device*		d_D3DDevice;
	IDXGISwapChain*		d_SwapChain;
	ID3D10Texture2D*	d_DepthStencilBuffer;
	ID3D10RenderTargetView* d_RenderTargetView;
	ID3D10DepthStencilView* d_DepthStencilView;
	ID3DX10Font* d_Font;

	// Hardware device or reference device.  Defualts to
	// D3D10_DRIVER_TYPE_HARDWARE
	D3D10_DRIVER_TYPE d_D3DDriverType;

	// Clear color
	D3DXCOLOR d_ClearColor;

	C_Graphics();
	C_Graphics(const C_Graphics&);
	static C_Graphics d_instance;

public:
	~C_Graphics();

	void onResize(const C_Window& window);
	bool initDirect3D(const C_Window& window);
	void clearBackBuffer();

	ID3D10Device* getDevice() const { return d_D3DDevice; } 

	void setClearColor(const D3DXCOLOR& color)	{ d_ClearColor = color; }
	void setDriverType(D3D10_DRIVER_TYPE type)	{ d_D3DDriverType = type; }

	void restoreDefualtStates();

	void presentFrame() { d_SwapChain->Present(0, 0); }

	void drawText(const std::wstring& text, const D3DXCOLOR& color, unsigned topLeftX, unsigned topLeftY, unsigned bottomRightX, unsigned bottomRightY);
};

extern C_Graphics* G_Graphics; 

#endif // _CGRAPHICS_
