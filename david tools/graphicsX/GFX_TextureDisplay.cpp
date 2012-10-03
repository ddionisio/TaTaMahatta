#include "gdix.h"
#include "gdix_i.h"

//this is only used by GFX_Texture
typedef struct _txtVertex {
    float x, y, z; //model space vertex position
	float nX, nY, nZ; //normal for lighting
    DWORD color;   //the color!
	float s, t;	   //texture coord
} txtVertex;

typedef struct _txtVSVertex {
    float x, y, z; //model space vertex position
	float s, t;	   //texture coord
} txtVSVertex;

#define TXT_VTXBUFF_USAGE	D3DUSAGE_WRITEONLY//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//
#define TXT_VTXBUFF_POOL	D3DPOOL_MANAGED//D3DPOOL_DEFAULT

#define MAXTXTVTX 4

#define GFXVSVERTEXFLAG (D3DFVF_XYZ | D3DFVF_TEX1) //xyz and texture

#define GFXTVERTEXFLAG (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1) //xyz and color and texture

LPD3DXSPRITE		    g_pTxtSprite=0; //global sprite interface
GFXVTXBUFF			    *g_pTxtVtxBuff=0; //global sprite 3d interface
GFXVTXBUFF			    *g_pTxtBBVtxBuff=0; //global Billboard 3d interface
GFXVTXBUFF				*g_pTxtDecalVtxBuff=0; //global Decal 3d interface

PGFXVERTEXSHADER		g_txtVShader=0;
PGFXVERTEXDECLARATION	g_txtVDecl=0;

//all texture creation/manipulation/destruction happens here...

////////////////////////////////////////////////////////////////////
// Very Simple Vertex Shader for Billboard and 3D texture drawing
PRIVATE void _TextureVSInit()
{
	LPD3DXBUFFER pCode;

	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	const char VSShader[] =
		"vs.1.1             //Shader version 1.1			\n"\
		"dcl_position v0									\n"\
		"dcl_texcoord v8									\n"\
		"m4x4 oPos, v0, c0  //emit projected position		\n"\
		"mov oT0, v8        //move texture				    \n"\
		"mov oD0, c4		//Diffuse color = c4            \n";

	//create the inputs required for this shader
	_GFXCheckError(g_p3DDevice->CreateVertexDeclaration(decl, &g_txtVDecl), 
		true, "Error in _TextureVSInit");

	//compile the vertex shader
	DWORD dwFlags = 0;

#if defined( _DEBUG ) || defined( DEBUG )
    dwFlags |= D3DXSHADER_DEBUG;
#endif

	_GFXCheckError(D3DXAssembleShader((LPCTSTR)VSShader,
    sizeof(VSShader), 0, 0, dwFlags, &pCode, 0),
	true, "Error in _TextureVSInit");

	//create the executable vertex shader
	_GFXCheckError(g_p3DDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),
		&g_txtVShader), true, "Error in _TextureVSInit");

	pCode->Release();
}

PRIVATE void _TextureVSTerm()
{
	if(g_txtVDecl)
	{ g_txtVDecl->Release(); g_txtVDecl = 0; }

	if(g_txtVShader)
	{ g_txtVShader->Release(); g_txtVShader = 0; }
}

PROTECTED void _TextureInitVtxBuff()
{
	//create sprite interface
	D3DXCreateSprite(
		g_p3DDevice,
		&g_pTxtSprite);
	
	//vertex shader stuff
	_TextureVSInit();

	//create the texture 3d vtx
	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(txtVSVertex)*NUMPTQUAD,
		TXT_VTXBUFF_USAGE, GFXVSVERTEXFLAG, TXT_VTXBUFF_POOL, &g_pTxtVtxBuff, 0),
	true, "Error in _TextureInitVtxBuff");

	//create the texture billboard vtx
	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(txtVSVertex)*NUMPTQUAD,
		TXT_VTXBUFF_USAGE, GFXVSVERTEXFLAG, TXT_VTXBUFF_POOL, &g_pTxtBBVtxBuff, 0),
	true, "Error in _TextureInitVtxBuff");

	//create the texture decal vtx
	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(txtVSVertex)*NUMPTQUAD,
		TXT_VTXBUFF_USAGE, GFXVSVERTEXFLAG, TXT_VTXBUFF_POOL, &g_pTxtDecalVtxBuff, 0),
	true, "Error in _TextureInitVtxBuff");

	txtVSVertex *pTVtx, *pTBBVtx, *pDecalVtx;

	//set as facing up from xz plane
	if(SUCCEEDED(g_pTxtVtxBuff->Lock(0,0,(void**)&pTVtx,0))
		&& SUCCEEDED(g_pTxtBBVtxBuff->Lock(0,0,(void**)&pTBBVtx,0))
		&& SUCCEEDED(g_pTxtDecalVtxBuff->Lock(0,0,(void**)&pDecalVtx,0)))
	{
		/*

			d-------c
			 |	   |
			 |     |
			 |     |
			a-------b

		*/

		pTVtx[0].x = -1.0f;  pTVtx[0].y = -1.0f; pTVtx[0].z = 0.0f;
		
		pTVtx[1].x = 1.0f;  pTVtx[1].y = -1.0f; pTVtx[1].z = 0.0f;
		
		pTVtx[2].x = 1.0f; pTVtx[2].y = 1.0f; pTVtx[2].z = 0.0f;
		
		pTVtx[3].x = -1.0f; pTVtx[3].y = 1.0f; pTVtx[3].z = 0.0f;

		//Top Left
		pTVtx[0].s = 0;
		pTVtx[0].t = 1;
		//Bottom Left
		pTVtx[1].s = 1;
		pTVtx[1].t = 1;
		//Bottom Right
		pTVtx[2].s = 1;
		pTVtx[2].t = 0;
		//Top Right
		pTVtx[3].s = 0;
		pTVtx[3].t = 0;

		memcpy(pTBBVtx, pTVtx, sizeof(txtVSVertex)*4);

		pDecalVtx[0].x = -1.0f;  pDecalVtx[0].y = 0; pDecalVtx[0].z = 1.0f;
		pDecalVtx[1].x = 1.0f;  pDecalVtx[1].y = 0; pDecalVtx[1].z = 1.0f;
		pDecalVtx[2].x = 1.0f; pDecalVtx[2].y = 0; pDecalVtx[2].z = -1.0f;
		pDecalVtx[3].x = -1.0f; pDecalVtx[3].y = 0; pDecalVtx[3].z = -1.0f;

		//Top Left
		pDecalVtx[0].s = 0; pDecalVtx[0].t = 1;
		//Bottom Left
		pDecalVtx[1].s = 1; pDecalVtx[1].t = 1;
		//Bottom Right
		pDecalVtx[2].s = 1; pDecalVtx[2].t = 0;
		//Top Right
		pDecalVtx[3].s = 0; pDecalVtx[3].t = 0;

		g_pTxtDecalVtxBuff->Unlock();
		g_pTxtBBVtxBuff->Unlock();
		g_pTxtVtxBuff->Unlock();
	}
}

PROTECTED void _TextureTermVtxBuff()
{
	_TextureVSTerm();

	if(g_pTxtSprite)
		g_pTxtSprite->Release();

	if(g_pTxtVtxBuff)
		g_pTxtVtxBuff->Release();

	if(g_pTxtBBVtxBuff)
		g_pTxtBBVtxBuff->Release();

	if(g_pTxtDecalVtxBuff)
		g_pTxtDecalVtxBuff->Release();

	g_pTxtSprite = 0;
	g_pTxtVtxBuff = 0;
	g_pTxtBBVtxBuff = 0;
	g_pTxtDecalVtxBuff = 0;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt(hTXT txt, float x, float y, const RECT *srcRect,
								 const DWORD *color, float rotate)
{
	D3DXVECTOR3 center(0,0,0);

	if(rotate != 0)
	{
		float sW, sH;
	
		if(srcRect)
		{ sW = (float)(srcRect->right - srcRect->left); sH = (float)(srcRect->bottom - srcRect->top); }
		else
		{ sW = (float)txt->width; sH = (float)txt->height; }

		D3DXMATRIX scaleRotOfsM(1,       0,       0,       0,
			                    0,       1,       0,       0,
							    0,       0,       1,       0,
							-sW/2,   -sH/2,       0,       1);

		D3DXMATRIX rotRotOfsInvTransM; D3DXMatrixRotationZ(&rotRotOfsInvTransM, rotate);
		
		rotRotOfsInvTransM._41 = sW/2;
		rotRotOfsInvTransM._42 = sH/2;
		rotRotOfsInvTransM._43 = 0;

		D3DXMATRIX transM; D3DXMatrixTranslation(&transM, x, y, 0);

		g_pTxtSprite->SetTransform(&D3DXMATRIX(scaleRotOfsM*rotRotOfsInvTransM*transM));
		g_pTxtSprite->Draw(txt->texture, srcRect, &center, 0, color ? *color : 0xffffffff);
	}
	else
	{
		D3DXVECTOR3 vect(x,y,0);

		D3DXMATRIX mtxIden; D3DXMatrixIdentity(&mtxIden);
		g_pTxtSprite->SetTransform(&mtxIden);
		g_pTxtSprite->Draw(txt->texture, srcRect, &center, &vect, color ? *color : 0xffffffff);
	}

	g_pTxtSprite->Flush();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureStretchBlt(hTXT txt, float x, float y, 
								 float w, float h, const RECT *srcRect,
								 const DWORD *color, float rotate)
{
	D3DXVECTOR3 center(0,0,0);

	float sW, sH;
	
	if(srcRect)
	{ sW = (float)(srcRect->right - srcRect->left); sH = (float)(srcRect->bottom - srcRect->top); }
	else
	{ sW = (float)txt->width; sH = (float)txt->height; }

	D3DXVECTOR3 scale(w/sW,h/sH,0);

	D3DXMATRIX scaleRotOfsM(scale.x, 0,       0,   0,
			            0,       scale.y, 0,       0,
						0,       0,       scale.z, 0,
						-w/2,       -h/2,       0,       1);

	D3DXMATRIX rotRotOfsInvTransM; D3DXMatrixRotationZ(&rotRotOfsInvTransM, rotate);
	
	rotRotOfsInvTransM._41 = w/2;
	rotRotOfsInvTransM._42 = h/2;
	rotRotOfsInvTransM._43 = 0;

	D3DXMATRIX transM; D3DXMatrixTranslation(&transM,x,y,0);

	g_pTxtSprite->SetTransform(&D3DXMATRIX(scaleRotOfsM*rotRotOfsInvTransM*transM));
	g_pTxtSprite->Draw(txt->texture, srcRect, &center, 0, color ? *color : 0xffffffff);

	g_pTxtSprite->Flush();

	/*float sW, sH;
	
	if(srcRect)
	{ sW = srcRect->right - srcRect->left; sH = srcRect->bottom - srcRect->top; }
	else
	{ sW = txt->width; sH = txt->height; }

	D3DXVECTOR2 vect(x,y);
	D3DXVECTOR2 scale(w/sW,h/sH);
	D3DXVECTOR2 rotCenter(w/2,h/2);

	g_pTxtSprite->Draw(txt->texture, srcRect, &scale, &rotCenter, rotate, &vect, color ? *color : 0xffffffff);
*/
	return RETCODE_SUCCESS;
}

PRIVATE void inline _TextureBlt3D(GFXVTXBUFF *vtxBuff, unsigned int vtxSize,
								  hTXT txt, const D3DXMATRIX *pWorldMtx,
								  float fClr[eMaxClr])
{
	g_p3DDevice->SetStreamSource(0, vtxBuff, 0, vtxSize);

	//set vertex shader stuff

	//projection matrix
	D3DXMATRIX vsMtx;
	D3DXMatrixMultiply(&vsMtx, pWorldMtx, &g_projMtx);
	D3DXMatrixTranspose(&vsMtx, &vsMtx);

	g_p3DDevice->SetVertexShaderConstantF(0, (float*)&vsMtx,  4);
	g_p3DDevice->SetVertexShaderConstantF(4, fClr,  1);

	g_p3DDevice->SetVertexDeclaration(g_txtVDecl);

	g_p3DDevice->SetVertexShader(g_txtVShader);

	//set texture
	TextureSet(txt, 0);

	//draw
    _GFXCheckError(g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2),
	true, "Error in TextureBlt3D");

	g_p3DDevice->SetVertexShader(0);

	g_p3DDevice->SetStreamSource(0,0,0,0);
	g_p3DDevice->SetTexture(0,0);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt3D(hTXT txt, const D3DXMATRIX *wrldMtx,
							float fClr[eMaxClr])
{
	if(fClr)
		_TextureBlt3D(g_pTxtVtxBuff, sizeof(txtVSVertex), txt, wrldMtx, fClr);
	else
	{
		float clr[eMaxClr] = {1,1,1,1};
		_TextureBlt3D(g_pTxtVtxBuff, sizeof(txtVSVertex), txt, wrldMtx, clr);
	}
	
	return RETCODE_SUCCESS;
}

//set up the billboard to face the camera
PROTECTED void TextureBillboardPrepare()
{
	txtVSVertex *pTVtx;

	D3DXMATRIX trans;

	g_p3DDevice->GetTransform(D3DTS_VIEW, &trans);

	float right[eMaxPt] = {trans._11, trans._21, trans._31};
	float up[eMaxPt] = {trans._12, trans._22, trans._32};

	if(SUCCEEDED(g_pTxtBBVtxBuff->Lock(0,0,(void**)&pTVtx,0)))
	{
		/*

			d-------c
			 |	   |
			 |     |
			 |     |
			a-------b

		a = center - (right + up) * size;
		b = center + (right - up) * size;
		c = center + (right + up) * size;
		d = center - (right - up) * size;

		*/

		pTVtx[0].x =  -(right[eX]+up[eX]);
		pTVtx[0].y =  -(right[eY]+up[eY]);
		pTVtx[0].z =  -(right[eZ]+up[eZ]);

		pTVtx[1].x =  (right[eX]-up[eX]);
		pTVtx[1].y =  (right[eY]-up[eY]);
		pTVtx[1].z =  (right[eZ]-up[eZ]);

		pTVtx[2].x =  (right[eX]+up[eX]);
		pTVtx[2].y =  (right[eY]+up[eY]);
		pTVtx[2].z =  (right[eZ]+up[eZ]);

		pTVtx[3].x =  -(right[eX]-up[eX]);
		pTVtx[3].y =  -(right[eY]-up[eY]);
		pTVtx[3].z =  -(right[eZ]-up[eZ]);

		g_pTxtBBVtxBuff->Unlock();
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBltBillboard(hTXT txt, const float center[eMaxPt],
								   const float size, float fClr[eMaxClr])
{
	D3DXMATRIX trans;

	//do transformations
	D3DXMatrixIdentity(&trans);

	trans._11 = size;
	trans._22 = size;
	trans._33 = size;

	trans._41 = center[eX];
    trans._42 = center[eY];
	trans._43 = center[eZ];

	if(fClr)
		_TextureBlt3D(g_pTxtBBVtxBuff, sizeof(txtVSVertex), txt, &trans, fClr);
	else
	{
		float clr[eMaxClr] = {1,1,1,1};
		_TextureBlt3D(g_pTxtBBVtxBuff, sizeof(txtVSVertex), txt, &trans, clr);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBltDecal(hTXT txt, const float center[eMaxPt],
								   const float size, 
								   const float norm[eMaxPt],
								   const RECT* pSrcRect, 
								   const GFXMATERIAL *material, const DWORD *color)
{
	D3DXMATRIX trans;

	//do transformations
	D3DXMatrixIdentity(&trans);

	trans._11 = size;
	trans._22 = size;
	trans._33 = size;

	trans._41 = center[eX];
    trans._42 = center[eY];
	trans._43 = center[eZ];

	g_p3DDevice->SetTransform(D3DTS_WORLD, &trans);

	////////////////////////////////////////////
	//HACK

	DWORD filterVal;
	GFXGetSampleState(0, D3DSAMP_MIPFILTER, &filterVal);
	GFXSetSampleState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	float fClr[eMaxClr] = {1, 1, 1, 1};

	_TextureBlt3D(g_pTxtDecalVtxBuff, sizeof(txtVSVertex), txt, &trans, fClr);

	GFXSetSampleState(0, D3DSAMP_MIPFILTER, filterVal);
	////////////////////////////////////////////

	return RETCODE_SUCCESS;
}