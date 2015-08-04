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
	d_SwapChain->SetFullscreenState(false, NULL);
	ReleaseCOM(d_RenderTargetView);
	ReleaseCOM(d_DepthStencilView);
	ReleaseCOM(d_SwapChain);
	ReleaseCOM(d_DepthStencilBuffer);
	ReleaseCOM(d_D3DDevice);
	ReleaseCOM(d_Font);
	ReleaseCOM(d_depthDisabledStencilState);
	ReleaseCOM(d_depthStencilState);
}

void C_Graphics::clearBackBuffer() {
	d_D3DDevice->ClearRenderTargetView(d_RenderTargetView, d_ClearColor);
	d_D3DDevice->ClearDepthStencilView(d_DepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void C_Graphics::drawText(const std::string& text, const D3DXCOLOR& color, unsigned topLeftX, unsigned topLeftY, unsigned bottomRightX, unsigned bottomRightY) {
	RECT R = {topLeftX, topLeftY, bottomRightX, bottomRightY};
	d_Font->DrawText(0, text.c_str(), -1, &R, DT_NOCLIP,  color);
}


bool C_Graphics::init(const C_Window& window, bool fullScreen) {
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
	sd.Windowed     = !fullScreen;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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
		DEBUGLOG("D3DDevice INIT FAILED");
		DXTrace(__FILE__, (DWORD)__LINE__, hr, 0, true);
		return false;
	}

	DEBUGLOG("D3DDevice INIT SUCCESSFUL");

	// The remaining steps that need to be carried out for D3D creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.
	onResize(window);

	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 16;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    //wcscpy_s(fontDesc.FaceName, "Times New Roman");
	strcpy_s(fontDesc.FaceName, "Courier New");

	hr = D3DX10CreateFontIndirect(d_D3DDevice, &fontDesc, &d_Font);

	if(FAILED(hr)) {
		DEBUGLOG("Font INIT FAILED");
		return false;
	}

	DEBUGLOG("Font INIT SUCCESSFUL");

	D3D10_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	
	hr = d_D3DDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &d_depthDisabledStencilState);
	if(FAILED(hr)) {
		DEBUGLOG("Depth Disabled Stencil State FAILED");
		return false;
	}

	DEBUGLOG("Depth Disabled Stencil State SUCCESSFUL");

	// Initialize the description of the stencil state.
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	hr = d_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &d_depthStencilState);
	if(FAILED(hr)) {
		DEBUGLOG("Depth Stencil State FAILED");
		return false;
	}

	DEBUGLOG("Depth Stencil State SUCCESSFUL");

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

void C_Graphics::enable2DMode() {
	d_D3DDevice->OMSetDepthStencilState(d_depthDisabledStencilState, 1);
}

void C_Graphics::enable3DMode() {
	d_D3DDevice->OMSetDepthStencilState(d_depthStencilState, 1);
}

void C_Graphics::setDefaultBlendStateAfterFontDraw() {
	// Restore default states, input layer, and primitive topology
	// because d_Font->DrawText changes them.  Note that we can restore
	// the default states by passing null
	d_D3DDevice->OMSetDepthStencilState(0,0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	d_D3DDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
}