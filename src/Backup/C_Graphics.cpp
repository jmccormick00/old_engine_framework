/*=========================================================================
/ James McCormick - C_Graphics.cpp
/ A class for managing the DirectX layer
/==========================================================================*/

#include "C_SystemLog.h"
#include "DXDebugUtility.h"
#include "C_Graphics.h"
#include "C_Window.h"
#include "UtilityMacros.h"


C_Graphics* G_Graphics;
C_Graphics C_Graphics::d_instance;

C_Graphics::C_Graphics() : d_D3DDevice(0), d_SwapChain(0), 
d_DepthStencilBuffer(0), d_RenderTargetView(0), d_DepthStencilView(0),
d_Font(0)
{G_Graphics = this;}

C_Graphics::~C_Graphics() {
	// Release the direct3D devices
	ReleaseCOM(d_RenderTargetView);
	ReleaseCOM(d_DepthStencilView);
	ReleaseCOM(d_SwapChain);
	ReleaseCOM(d_DepthStencilBuffer);
	ReleaseCOM(d_D3DDevice);
	ReleaseCOM(d_Font);
	//G_Engine->PostToLog(L"Released D3DDevice");
}

void C_Graphics::clearBackBuffer() {
	d_D3DDevice->ClearRenderTargetView(d_RenderTargetView, d_ClearColor);
	d_D3DDevice->ClearDepthStencilView(d_DepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}


void C_Graphics::restoreDefualtStates() {
	// Restore default states, input layer, and primitive topology
	// because d_Font->DrawText changes them.  Note that we can restore
	// the default states by passing null
	d_D3DDevice->OMSetDepthStencilState(0,0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	d_D3DDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
}


void C_Graphics::drawText(const std::wstring& text, const D3DXCOLOR& color, unsigned topLeftX, unsigned topLeftY, unsigned bottomRightX, unsigned bottomRightY) {
	RECT R = {topLeftX, topLeftY, bottomRightX, bottomRightY};
	d_Font->DrawText(0, text.c_str(), -1, &R, DT_NOCLIP,  color);
}


bool C_Graphics::initDirect3D(const C_Window& window) {
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferDesc.Width  = window.getWidth();
	sd.BufferDesc.Height = window.getHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// No multisampling
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = window.getWindowHandle();
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;


	// Create the device
	UINT createDeviceFlags = 0;
	//#if defined(DEBUG) || defined(_DEBUG)  
	//	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	//#endif
	HRESULT hr =
	D3D10CreateDeviceAndSwapChain(
			0,                 //default adapter
			d_D3DDriverType,
			0,                 // no software device
			createDeviceFlags, 
			D3D10_SDK_VERSION,
			&sd,
			&d_SwapChain,
			&d_D3DDevice);

	if(FAILED(hr)) {
		DEBUGLOG(L"D3DDevice INIT FAILED");
		DXTrace(__FILE__, (DWORD)__LINE__, hr, 0, true);
		return false;
	}

	DEBUGLOG(L"D3DDevice INIT SUCCESSFUL");

	// The remaining steps that need to be carried out for D3D creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.
	onResize(window);

	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 14;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(fontDesc.FaceName, L"Times New Roman");

	hr = D3DX10CreateFontIndirect(d_D3DDevice, &fontDesc, &d_Font);

	if(FAILED(hr)) {
		DEBUGLOG(L"Font INIT FAILED");
		return false;
	}

	DEBUGLOG(L"Font INIT SUCCESSFUL");

	// Init the defualt shader
	//d_pDefaultShader = new C_DefaultShader(d_D3DDevice);
	return true;
}



void C_Graphics::onResize(const C_Window& window) {
	// Release the old views, as they hold references to old devices
	// Also release the old depth/stencil buffer.
	ReleaseCOM(d_RenderTargetView);
	ReleaseCOM(d_DepthStencilView);
	ReleaseCOM(d_DepthStencilBuffer);

	// Resize the swap chain and recreate the render target view.
	HR(d_SwapChain->ResizeBuffers(1, window.getWidth(), window.getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D10Texture2D* backBuffer;
	HR(d_SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(d_D3DDevice->CreateRenderTargetView(backBuffer, 0, &d_RenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view
	D3D10_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = window.getWidth();
	depthStencilDesc.Height    = window.getHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HR(d_D3DDevice->CreateTexture2D(&depthStencilDesc, 0, &d_DepthStencilBuffer));
	HR(d_D3DDevice->CreateDepthStencilView(d_DepthStencilBuffer, 0, &d_DepthStencilView));

	// Bind the render target view and depth/stencil view to the pipeline
	d_D3DDevice->OMSetRenderTargets(1, &d_RenderTargetView, d_DepthStencilView);

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = window.getWidth();
	vp.Height   = window.getHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	d_D3DDevice->RSSetViewports(1, &vp);
}
