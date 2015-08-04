/*=========================================================================
/ James McCormick - C_DefualtShader.h
/ The default shader class
/==========================================================================*/

#ifndef _DEFAULTSHADER_
#define _DEFAULTSHADER_

#include "C_Light.h"

class C_DefaultShader {
private:
	ID3D10Device* d_pDevice;

	ID3D10Effect* d_pFX;
	ID3D10EffectTechnique* d_pTextureTech;
	ID3D10EffectTechnique* d_pNoTextureTech;

	ID3D10InputLayout* d_pTextureVertexLayout;
	ID3D10InputLayout* d_pNoTextureVertexLayout;

	// Shader global variables
	ID3D10EffectMatrixVariable* d_fxViewProjVar;		// The view projection matrix
	ID3D10EffectMatrixVariable* d_fxWorld;				// The world matrix
	ID3D10EffectVectorVariable* d_fxEyePosVar;			// The eye pos of the camera
	ID3D10EffectVariable* d_fxLightVar;					// The light array 
	ID3D10EffectVectorVariable* d_fxLightParams;		// The params for the light array
	ID3D10EffectVectorVariable* d_fxTextureParams;		// The params for the texture(s)
	ID3D10EffectShaderResourceVariable* d_fxTexture;	// The texture variable

	void buildFX();
	void buildVertexLayout();

	// Prevent from using any other way except the constructor
	C_DefaultShader() {}
	C_DefaultShader(const C_DefaultShader&) {}

public:

	struct S_DefaultTextureVertex {
		S_DefaultTextureVertex() {}
		S_DefaultTextureVertex(float x, float y, float z, 
			float nx, float ny, float nz, 
			float u, float v)
			: pos(x,y,z), normal(nx,ny,nz), texC(u,v) {}

		Math::vector3 pos;
		Math::vector3 normal;
		D3DXVECTOR2 texC;
	};

	struct S_DefaultNoTextureVertex {
		S_DefaultNoTextureVertex() {}
		S_DefaultNoTextureVertex(float x, float y, float z, 
			float nx, float ny, float nz, float r, float g, float b)
			: pos(x,y,z), normal(nx,ny,nz), color(r, g, b)  {}

		Math::vector3 pos;
		Math::vector3 normal;
		Math::vector3 color;
	};

	C_DefaultShader(ID3D10Device* device);
	~C_DefaultShader();

	ID3D10EffectPass* getTexturePassByIndex(UINT i) const { d_pTextureTech->GetPassByIndex(i); }
	ID3D10EffectPass* getNoTexturePassByIndex(UINT i) const { d_pNoTextureTech->GetPassByIndex(i); }

	ID3D10Effect* getFX() const { return d_pFX; }
	ID3D10EffectTechnique* getNoTextureTech() const { return d_pNoTextureTech; }
	ID3D10EffectTechnique* getTextureTech() const { return d_pTextureTech; }

	void setEyePos(const Math::vector3& eye)				
	{ d_fxEyePosVar->SetRawValue((void*)&eye, 0, sizeof(Math::vector3)); }

	void setWorld(const Math::matrix4x4& world)
	{ d_fxWorld->SetMatrix((float*)&world); }

	void setViewProj(const Math::matrix4x4& viewProj)	
	{ d_fxViewProjVar->SetMatrix((float*)&viewProj); }

	void setTexture(ID3D10ShaderResourceView* textureRV) { 
		if(d_fxTexture)
			d_fxTexture->SetResource(textureRV); 
	}

	void setTextureArray(ID3D10ShaderResourceView** texArray, UINT uiCount)
	{ d_fxTexture->SetResourceArray(texArray, 0, uiCount); }

	void setNumActiveTexture(int texCount, int iCurrent);

	void setLights(C_Light* pLights, int numLights);
};

#endif // DEFAULTSHADER
