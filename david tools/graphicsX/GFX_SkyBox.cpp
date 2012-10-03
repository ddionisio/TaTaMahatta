#include "gdix.h"
#include "gdix_i.h"

typedef enum {
	eSkyTop,
	eSkyBottom,
	eSkyFront,
	eSkyBack,
	eSkyLeft,
	eSkyRight,
	eSkyNumSide
} SkySides;

#define SKY_D3DUSAGE		D3DUSAGE_WRITEONLY
#define SKY_D3DPOOL			D3DPOOL_MANAGED//D3DPOOL_DEFAULT

//this is only used by GFX_Texture
typedef struct _skyVertex {
    float x, y, z; //model space vertex position
	float s, t;	   //texture coord
} skyVertex;

#define GFXSKYVERTEXFLAG (D3DFVF_XYZ | D3DFVF_TEX1) //xyz and texture

GFXVTXBUFF			    *g_pSkyVtxBuff[eSkyNumSide]={0};

hTXT					 g_skyTextures[eSkyNumSide]={0};

D3DXMATRIX				 g_skyTrans; //just scaling

PRIVATE bool _SkyBoxValid()
{
	for(int i = 0; i < eSkyNumSide; i++)
	{
		if(g_skyTextures[i] == 0 || g_pSkyVtxBuff[i] == 0)
			return false;
	}

	return true;
}

/////////////////////////////////////
// Name:	SkyBoxDestroy
// Purpose:	destroys the skybox
// Output:	skybox destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void SkyBoxDestroy()
{
	for(int i = 0; i < eSkyNumSide; i++)
	{
		if(g_pSkyVtxBuff[i])
		{ g_pSkyVtxBuff[i]->Release(); g_pSkyVtxBuff[i] = 0; }

		if(g_skyTextures[i])
		{ TextureDestroy(&g_skyTextures[i]); g_skyTextures[i] = 0; }
	}
}

/*

	d-------c
	 |	   |
	 |     |
	 |     |
	a-------b

*/

PRIVATE inline void _SkyBoxInitTop()
{
	//top (x/z)

	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(skyVertex)*NUMPTQUAD,
		SKY_D3DUSAGE, GFXSKYVERTEXFLAG, SKY_D3DPOOL, &g_pSkyVtxBuff[eSkyTop], 0),
	true, "SkyBoxInit");

	skyVertex	*pSkyVtx;

	//set as facing up from xz plane
	if(SUCCEEDED(g_pSkyVtxBuff[eSkyTop]->Lock(0,0,(void**)&pSkyVtx,0)))
	{
		pSkyVtx[0].x = -1.0f;  pSkyVtx[0].y = 1.0f; pSkyVtx[0].z = -1.0f;
		pSkyVtx[0].s = 0;      pSkyVtx[0].t = 1;

		pSkyVtx[1].x = 1.0f;   pSkyVtx[1].y = 1.0f; pSkyVtx[1].z = -1.0f;
		pSkyVtx[1].s = 1;      pSkyVtx[1].t = 1;

		pSkyVtx[2].x = 1.0f;   pSkyVtx[2].y = 1.0f; pSkyVtx[2].z = 1.0f;
		pSkyVtx[2].s = 1;      pSkyVtx[2].t = 0;

		pSkyVtx[3].x = -1.0f;  pSkyVtx[3].y = 1.0f; pSkyVtx[3].z = 1.0f;
		pSkyVtx[3].s = 0;      pSkyVtx[3].t = 0;

		g_pSkyVtxBuff[eSkyTop]->Unlock();
	}
}

PRIVATE inline void _SkyBoxInitBottom()
{
	//bottom (x/z)

	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(skyVertex)*NUMPTQUAD,
		SKY_D3DUSAGE, GFXSKYVERTEXFLAG, SKY_D3DPOOL, &g_pSkyVtxBuff[eSkyBottom], 0),
	true, "SkyBoxInit");

	skyVertex	*pSkyVtx;

	//set as facing down from xz plane
	if(SUCCEEDED(g_pSkyVtxBuff[eSkyBottom]->Lock(0,0,(void**)&pSkyVtx,0)))
	{
		pSkyVtx[0].x = -1.0f;  pSkyVtx[0].y = -1.0f; pSkyVtx[0].z = -1.0f;
		pSkyVtx[0].s = 0;      pSkyVtx[0].t = 0;

		pSkyVtx[3].x = 1.0f;   pSkyVtx[3].y = -1.0f; pSkyVtx[3].z = -1.0f;
		pSkyVtx[3].s = 1;      pSkyVtx[3].t = 0;

		pSkyVtx[2].x = 1.0f;   pSkyVtx[2].y = -1.0f; pSkyVtx[2].z = 1.0f;
		pSkyVtx[2].s = 1;      pSkyVtx[2].t = 1;

		pSkyVtx[1].x = -1.0f;  pSkyVtx[1].y = -1.0f; pSkyVtx[1].z = 1.0f;
		pSkyVtx[1].s = 0;      pSkyVtx[1].t = 1;

		g_pSkyVtxBuff[eSkyBottom]->Unlock();
	}
}

PRIVATE inline void _SkyBoxInitFront()
{
	//front (x/y)

	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(skyVertex)*NUMPTQUAD,
		SKY_D3DUSAGE, GFXSKYVERTEXFLAG, SKY_D3DPOOL, &g_pSkyVtxBuff[eSkyFront], 0),
	true, "SkyBoxInit");

	skyVertex	*pSkyVtx;

	//set as facing front from xy plane
	if(SUCCEEDED(g_pSkyVtxBuff[eSkyFront]->Lock(0,0,(void**)&pSkyVtx,0)))
	{
		pSkyVtx[0].x = -1.0f;  pSkyVtx[0].y = -1.0f; pSkyVtx[0].z = -1.0f; 
		pSkyVtx[0].s = 0;      pSkyVtx[0].t = 1;

		pSkyVtx[1].x = 1.0f;   pSkyVtx[1].y = -1.0f; pSkyVtx[1].z = -1.0f; 
		pSkyVtx[1].s = 1;      pSkyVtx[1].t = 1;

		pSkyVtx[2].x = 1.0f;   pSkyVtx[2].y = 1.0f;  pSkyVtx[2].z = -1.0f; 
		pSkyVtx[2].s = 1;      pSkyVtx[2].t = 0;

		pSkyVtx[3].x = -1.0f;  pSkyVtx[3].y = 1.0f;  pSkyVtx[3].z = -1.0f; 
		pSkyVtx[3].s = 0;      pSkyVtx[3].t = 0;

		g_pSkyVtxBuff[eSkyFront]->Unlock();
	}
}

PRIVATE inline void _SkyBoxInitBack()
{
	//back (x/y)

	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(skyVertex)*NUMPTQUAD,
		SKY_D3DUSAGE, GFXSKYVERTEXFLAG, SKY_D3DPOOL, &g_pSkyVtxBuff[eSkyBack], 0),
	true, "SkyBoxInit");

	skyVertex	*pSkyVtx;

	//set as facing back from xy plane
	if(SUCCEEDED(g_pSkyVtxBuff[eSkyBack]->Lock(0,0,(void**)&pSkyVtx,0)))
	{
		pSkyVtx[0].x = -1.0f;  pSkyVtx[0].y = -1.0f; pSkyVtx[0].z = 1.0f; 
		pSkyVtx[0].s = 1;      pSkyVtx[0].t = 1;

		pSkyVtx[3].x = 1.0f;   pSkyVtx[3].y = -1.0f; pSkyVtx[3].z = 1.0f; 
		pSkyVtx[3].s = 0;      pSkyVtx[3].t = 1;

		pSkyVtx[2].x = 1.0f;   pSkyVtx[2].y = 1.0f;  pSkyVtx[2].z = 1.0f; 
		pSkyVtx[2].s = 0;      pSkyVtx[2].t = 0;

		pSkyVtx[1].x = -1.0f;  pSkyVtx[1].y = 1.0f;  pSkyVtx[1].z = 1.0f; 
		pSkyVtx[1].s = 1;      pSkyVtx[1].t = 0;

		g_pSkyVtxBuff[eSkyBack]->Unlock();
	}
}

PRIVATE inline void _SkyBoxInitLeft()
{
	//left (y/z)

	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(skyVertex)*NUMPTQUAD,
		SKY_D3DUSAGE, GFXSKYVERTEXFLAG, SKY_D3DPOOL, &g_pSkyVtxBuff[eSkyLeft], 0),
	true, "SkyBoxInit");

	skyVertex	*pSkyVtx;

	//set as facing left from yz plane
	if(SUCCEEDED(g_pSkyVtxBuff[eSkyLeft]->Lock(0,0,(void**)&pSkyVtx,0)))
	{
		pSkyVtx[0].x = -1.0f; pSkyVtx[0].y = -1.0f; pSkyVtx[0].z = -1.0f;
		pSkyVtx[0].s = 1;     pSkyVtx[0].t = 1;

		pSkyVtx[1].x = -1.0f; pSkyVtx[1].y = 1.0f;  pSkyVtx[1].z = -1.0f;
		pSkyVtx[1].s = 1;     pSkyVtx[1].t = 0;

		pSkyVtx[2].x = -1.0f; pSkyVtx[2].y = 1.0f;  pSkyVtx[2].z = 1.0f;
		pSkyVtx[2].s = 0;     pSkyVtx[2].t = 0;

		pSkyVtx[3].x = -1.0f; pSkyVtx[3].y = -1.0f; pSkyVtx[3].z = 1.0f;
		pSkyVtx[3].s = 0;     pSkyVtx[3].t = 1;

		g_pSkyVtxBuff[eSkyLeft]->Unlock();
	}
}

PRIVATE inline void _SkyBoxInitRight()
{
	//right (y/z)

	_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(skyVertex)*NUMPTQUAD,
		SKY_D3DUSAGE, GFXSKYVERTEXFLAG, SKY_D3DPOOL, &g_pSkyVtxBuff[eSkyRight], 0),
	true, "SkyBoxInit");

	skyVertex	*pSkyVtx;

	//set as facing right from yz plane
	if(SUCCEEDED(g_pSkyVtxBuff[eSkyRight]->Lock(0,0,(void**)&pSkyVtx,0)))
	{
		pSkyVtx[0].x = 1.0f;  pSkyVtx[0].y = -1.0f; pSkyVtx[0].z = -1.0f;
		pSkyVtx[0].s = 0;     pSkyVtx[0].t = 1;

		pSkyVtx[3].x = 1.0f;  pSkyVtx[3].y = 1.0f;  pSkyVtx[3].z = -1.0f;
		pSkyVtx[3].s = 0;     pSkyVtx[3].t = 0;

		pSkyVtx[2].x = 1.0f;  pSkyVtx[2].y = 1.0f;  pSkyVtx[2].z = 1.0f;
		pSkyVtx[2].s = 1;     pSkyVtx[2].t = 0;

		pSkyVtx[1].x = 1.0f;  pSkyVtx[1].y = -1.0f; pSkyVtx[1].z = 1.0f;
		pSkyVtx[1].s = 1;     pSkyVtx[1].t = 1;

		g_pSkyVtxBuff[eSkyRight]->Unlock();
	}
}

/////////////////////////////////////
// Name:	SkyBoxInit
// Purpose:	initializes skybox with
//			given textures for each
//			side
// Output:	skybox init
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE SkyBoxInit(float scale,
						  hTXT textureTop, hTXT textureBottom,
						  hTXT textureFront, hTXT textureBack,
						  hTXT textureLeft, hTXT textureRight)
{
	//destroy old skybox
	SkyBoxDestroy();

	//set scaling mtx
	D3DXMatrixScaling(&g_skyTrans, scale, scale, scale);


	//set the textures
	g_skyTextures[eSkyTop]    = textureTop;    TextureAddRef(g_skyTextures[eSkyTop]);
	g_skyTextures[eSkyBottom] = textureBottom; TextureAddRef(g_skyTextures[eSkyBottom]);
	g_skyTextures[eSkyFront]  = textureFront;  TextureAddRef(g_skyTextures[eSkyFront]);
	g_skyTextures[eSkyBack]   = textureBack;   TextureAddRef(g_skyTextures[eSkyBack]);
	g_skyTextures[eSkyLeft]   = textureLeft;   TextureAddRef(g_skyTextures[eSkyLeft]);
	g_skyTextures[eSkyRight]  = textureRight;  TextureAddRef(g_skyTextures[eSkyRight]);

	//set the sides
	_SkyBoxInitTop();
	_SkyBoxInitBottom();
	_SkyBoxInitFront();
	_SkyBoxInitBack();
	_SkyBoxInitLeft();
	_SkyBoxInitRight();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	SkyBoxRender
// Purpose:	renders the skybox
// Output:	sky box rendered
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE SkyBoxRender()
{
	if(!_SkyBoxValid()) //that means skybox hasn't been initialized
		return RETCODE_SUCCESS;

	//////////////////////////////////////////////////
	DWORD txtValU, txtValV;

	GFXGetSampleState(0, D3DSAMP_ADDRESSU, &txtValU);
	GFXGetSampleState(0, D3DSAMP_ADDRESSV, &txtValV);

	if(txtValU != D3DTADDRESS_CLAMP)
		GFXSetSampleState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

	if(txtValV != D3DTADDRESS_CLAMP)
		GFXSetSampleState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	//////////////////////////////////////////////////

	if(GFXIsLightEnabled())
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	/////////////////////////////////////////////////
	D3DXMATRIX viewMtx;

	g_p3DDevice->GetTransform(D3DTS_VIEW, &viewMtx);

	D3DXMATRIX skyViewMtx(viewMtx);

	skyViewMtx._41 = 0; skyViewMtx._42 = 0; skyViewMtx._43 = 0;

	g_p3DDevice->SetTransform(D3DTS_VIEW, &skyViewMtx);

	g_p3DDevice->SetTransform(D3DTS_WORLD, &g_skyTrans);
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	g_p3DDevice->SetFVF(GFXSKYVERTEXFLAG);

	for(int i = 0; i < eSkyNumSide; i++)
	{
		//set texture
		TextureSet(g_skyTextures[i], 0);

		g_p3DDevice->SetStreamSource(0, g_pSkyVtxBuff[i], 0, sizeof(skyVertex));

	
		_GFXCheckError(g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2),
		true, "SkyBoxRender");

		g_p3DDevice->SetStreamSource(0,0,0,0);
		g_p3DDevice->SetTexture(0,0);
	}
	/////////////////////////////////////////////////

	g_p3DDevice->SetTransform(D3DTS_VIEW, &viewMtx);

	//enable light again, if flag is set
    if(GFXIsLightEnabled())
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	GFXSetSampleState(0, D3DSAMP_ADDRESSU, txtValU);
	GFXSetSampleState(0, D3DSAMP_ADDRESSV, txtValV);

	return RETCODE_SUCCESS;
}