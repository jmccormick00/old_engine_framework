
#include "DXDebugUtility.h"
#include "UtilityMacros.h"
#include "C_DefaultShader.h"

#define DEFAULT_SHADER_FILE L"DefaultShader.fx"


C_DefaultShader::C_DefaultShader(ID3D10Device *device) {
	d_pDevice = device;

	buildFX();
	buildVertexLayout();
}


C_DefaultShader::~C_DefaultShader() {

}


void C_DefaultShader::buildFX() {
	// Create the Default shader
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(DEFAULT_SHADER_FILE, 0, 0, 
		"fx_4_0", shaderFlags, 0, d_pDevice, 0, 0, &d_pFX, &compilationErrors, 0);   // TODO - make the effect file in the header file as a string and use D3DX10CreateEffectFromMemory()
	if(FAILED(hr)) {
		if( compilationErrors ) {
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	d_pTextureTech = d_pFX->GetTechniqueByName("TextureDefaultTech");
	d_pNoTextureTech = d_pFX->GetTechniqueByName("NoTextureDefaultTech");
	
	d_fxViewProjVar     = d_pFX->GetVariableByName("gVP")->AsMatrix();
	d_fxWorld			= d_pFX->GetVariableByName("gWorld")->AsMatrix();
	d_fxEyePosVar		= d_pFX->GetVariableByName("gEyePosW")->AsVector();
	d_fxTextureParams	= d_pFX->GetVariableByName("gTextureParams")->AsVector();
	d_fxLightParams		= d_pFX->GetVariableByName("gLightParams")->AsVector();
	d_fxTexture			= d_pFX->GetVariableByName("gTexture")->AsShaderResource();
	d_fxLightVar		= d_pFX->GetVariableByName("gLights");
}

void C_DefaultShader::buildVertexLayout() {
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
	d_pTextureTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(d_pDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &d_pTextureVertexLayout));

	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc1[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    d_pNoTextureTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(d_pDevice->CreateInputLayout(vertexDesc1, 3, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &d_pNoTextureVertexLayout));
}


void C_DefaultShader::setNumActiveTexture(int texCount, int iCurrent) {
	int data[4] = {texCount, iCurrent, 0, 0};
	d_fxTextureParams->SetIntVector(data);
}


//void C_DefaultShader::setLights(const C_Light const* pLights, int numLights) { // TODO - Finish
//	//int data[4] = 
//	//d_fxLightParams->SetRawValue((void*)&eye, 0, sizeof(vector3));
//	//d_fxLightVar;
//}
