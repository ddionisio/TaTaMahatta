#include "gdix.h"
#include "gdix_i.h"

#include "gdix_screen.h"

#pragma comment(lib,"d3d9.lib") //no linking needed
#pragma comment(lib,"d3dx9.lib")
//#pragma comment(lib,"winmm.lib")

#ifdef _DEBUG
int g_numCulled=0;
int g_numFaceProcessed=0;
#endif

gfxDisplayMode	  g_displayMode={0};

PGFXMAIN		  g_pD3D = 0;
PGFXDEVICE		  g_p3DDevice = 0;
ID3DXMatrixStack *g_pWrldStack = 0;//the world matrix stack
//gfxDisplayMode	  g_DISPLAY = {0};
RECT			  g_CLIENTRECT = {0,0,0,0};//window rectangle (only useful in window mode)
FLAGS			  g_FLAGS = 0;

LISTPTR			  g_TXTLIST;			//list of textures
LISTPTR			  g_MDLLIST;			//list of models
LISTPTR			  g_OBJLIST;			//list of objects
LISTPTR			  g_OBJDEACTIVATELIST;	//list of deactivated objects

char			  g_MDLLISTFILE[DMAXCHARBUFF]={0}; //the model list file
char			  g_TXTLISTFILE[DMAXCHARBUFF]={0}; //the texture list file

float g_sinTable[SINCOUNT]={0};

GFXCAPS g_d3dcaps;

D3DXMATRIX g_projMtx;		//concatinated view and proj mtx. (view*proj.)
							//updated whenever camera is set.

D3DXVECTOR3		g_eyePt;

D3DPRESENT_PARAMETERS g_d3dPresentParam={0};

PRIVATE void _GFXInitSinTable()
{
	for(int i = 0; i < SINCOUNT; i++)
	{
		g_sinTable[i] = float(sin(((2*D3DX_PI)/SINCOUNT)*i));
	}
}

bool g_bDeviceLost = false;

/////////////////////////////////////
// Name:	_GFXGetD3dFormat
// Purpose:	grabs D3DFORMAT of pMode
// Output:	none
// Return:	the D3DFORMAT
/////////////////////////////////////
PROTECTED D3DFORMAT _GFXGetD3dFormat(gfxDisplayMode *pMode)
{
	if(pMode)
	{
		switch(pMode->bpp) //get d3dformat, screw the rest...
		{
		case BPP_32:
			//return D3DFMT_A8R8G8B8;
			return D3DFMT_X8R8G8B8;
		case BPP_24:
			return D3DFMT_R8G8B8;
		case BPP_16:
			return D3DFMT_R5G6B5;
		case BPP_15a:
			return D3DFMT_A1R5G5B5;
		case BPP_15:
			return D3DFMT_X1R5G5B5;
		case BPP_8:
			return D3DFMT_P8;
		case BPP_DOH:
			//get format of choice...
			return D3DFMT_UNKNOWN;
		}
	}

	return D3DFMT_UNKNOWN;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC bool GFXIsDepthFormatExisting(D3DDEVTYPE device, D3DFORMAT DepthFormat, D3DFORMAT AdapterFormat)
{
    HRESULT hr = g_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                          device,
                                          AdapterFormat,
                                          D3DUSAGE_DEPTHSTENCIL,
                                          D3DRTYPE_SURFACE,
                                          DepthFormat);

    return SUCCEEDED( hr );
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXResize(unsigned int width, unsigned int height)
{
	//set up the projection matrix
	D3DXMATRIX mtxProj;

	D3DXMatrixPerspectiveFovRH(&mtxProj, //Result Matrix
                              FOV,		 //Use 45 degree
                              ((float)width / (float)height),     //Aspect ratio
                              kDEPTHNEAR,     //Near view plane
                              kDEPTHFAR);	  // Far view plane

   //set up the projection matrix, we won't be doing anything else...
   g_p3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProj);

   g_projMtx = mtxProj;

   //set up the screen interface thing
   //if(!g_bDeviceLost)
   {
	   if(g_d3dPresentParam.Windowed == TRUE)
	   {
		   D3DDISPLAYMODE		  d3dDisplayMode={0};
			//Get desktop display mode.
			if(_GFXCheckError(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), true, "Error in GFXInit"))
				return;

			GFX_ScreenCreate(d3dDisplayMode.Width, d3dDisplayMode.Height);
	   }
	   else
		   GFX_ScreenCreate(width, height);
   }
}

/////////////////////////////////////
// Name:	GFXSetTextureStageState/
//			GFXGetTextureStageState
// Purpose:	set/get texture stage state
// Output:	val filled if get
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE GFXSetTextureStageState(DWORD base,
									   DWORD type,
									   DWORD val)
{
	g_p3DDevice->SetTextureStageState(base,(D3DTEXTURESTAGESTATETYPE)type,val);

	return RETCODE_SUCCESS;
}

PUBLIC void GFXGetTextureStageState(DWORD base,
									   DWORD type,
									   DWORD *val)
{
	g_p3DDevice->GetTextureStageState(base,(D3DTEXTURESTAGESTATETYPE)type,val);
}

/////////////////////////////////////
// Name:	GFXSetSampleState/
//			GFXGetSampleState
// Purpose:	set/get sample state
// Output:	val filled if get
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE GFXSetSampleState(DWORD sampler,
								 DWORD type,
								 DWORD val)
{
	g_p3DDevice->SetSamplerState(sampler,(D3DSAMPLERSTATETYPE)type,val);

	return RETCODE_SUCCESS;
}

PUBLIC void GFXGetSampleState(DWORD sampler,
							  DWORD type,
							  DWORD *val)
{
	g_p3DDevice->GetSamplerState(sampler,(D3DSAMPLERSTATETYPE)type,val);
}

/////////////////////////////////////
// Name:	GFXSetRenderState/
//			GFXGetRenderState
// Purpose:	set/get sample state
// Output:	val filled if get
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE GFXSetRenderState(DWORD state, DWORD val)
{
	g_p3DDevice->SetRenderState((D3DRENDERSTATETYPE)state,val);

	return RETCODE_SUCCESS;
}

PUBLIC void GFXGetRenderState(DWORD state, DWORD *val)
{
	g_p3DDevice->GetRenderState((D3DRENDERSTATETYPE)state,val);
}

PRIVATE void _GFXSetDefaultStates()
{
	//set up depth buffering
	g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	g_p3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	/////////////////////////////////////////////////////////////////////////////////
	//set up base texture rendering/blending
	//this tells us to set texture color operation as modulate: COLORARG1 * COLORARG2
	GFXSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	GFXSetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE); //use texture
    GFXSetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT); //use vtx color

	GFXSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	GFXSetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE); //use texture
    GFXSetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_CURRENT); //use vtx color

	//set filtering, TODO: check for support...
	GFXSetSampleState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	GFXSetSampleState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	GFXSetSampleState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	/////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////
	GFXSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);

	GFXSetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_CURRENT); //use texture
	GFXSetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_TEXTURE);

	GFXSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	GFXSetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE); //use texture
    GFXSetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_CURRENT); //use vtx color

	//set filtering, TODO: check for support...
	GFXSetSampleState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	GFXSetSampleState(1,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	GFXSetSampleState(1,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	/////////////////////////////////////////////////////////////////////////////////

	//set up alpha blending for renderer
	g_p3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
    g_p3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//set material mode
	g_p3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    g_p3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    g_p3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    g_p3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	//set specular highlight, why the hell not?
	//g_p3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	//g_p3DDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
	 

	//g_p3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    //g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	///////////////////////////////////////////////////////////////
	//Set up alpha test
	//g_p3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE , TRUE);
	//g_p3DDevice->SetRenderState(D3DRS_ALPHAFUNC , D3DCMP_GREATER  );

    g_p3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	//set the texture transformation states
	/*D3DTEXTURETRANSFORMFLAGS tflag = (D3DTEXTURETRANSFORMFLAGS)eMaxTxtPt;

	for(int i = 0; i < NUMVTXTEXTURE; i++)
	{
		HRESULT hr = g_p3DDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, tflag);
	}*/
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXInit(HWND hwnd, gfxDisplayMode *pMode)
{
	//quickly initialize the sin table first
	_GFXInitSinTable();

	//the very first thing to do
	//initialize the mac daddy
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if(!g_pD3D) //make sure it's successfully created
	{ ASSERT_MSG(0, "Unable to create 3D Device", "Error in GFXInit"); return RETCODE_FAILURE; }

	//default driver for now...
	if(FAILED(g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &g_d3dcaps)))
		g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &g_d3dcaps);


	if(g_d3dcaps.MaxActiveLights > 8)
		g_d3dcaps.MaxActiveLights = 8;

	D3DPRESENT_PARAMETERS d3dParam={0}; 

	d3dParam.SwapEffect     = D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_DISCARD;//
    d3dParam.hDeviceWindow  = hwnd;					 //our HWND
    d3dParam.BackBufferCount= 1;					 //use only 1
	d3dParam.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dParam.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	if(pMode)
	{
		SETFLAG(g_FLAGS, GFX_FULLSCREEN); //set up the flag

		d3dParam.Windowed          = FALSE;
		d3dParam.BackBufferWidth   = pMode->width;
		d3dParam.BackBufferHeight  = pMode->height;
		d3dParam.BackBufferFormat  = _GFXGetD3dFormat(pMode);
		d3dParam.FullScreen_RefreshRateInHz = pMode->refresh;

		memcpy(&g_displayMode, pMode, sizeof(g_displayMode));
	}
	else
	{
		D3DDISPLAYMODE		  d3dDisplayMode={0};
		//Get desktop display mode.
	    if(_GFXCheckError(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), true, "Error in GFXInit"))
			return RETCODE_FAILURE;

		d3dParam.Windowed          = TRUE;
		d3dParam.BackBufferFormat  = d3dDisplayMode.Format;

		//get client area
		RECT winRect; GetClientRect(hwnd, &winRect);
		g_displayMode.width = winRect.right-winRect.left;
		g_displayMode.height = winRect.bottom-winRect.top;
	}

	d3dParam.EnableAutoDepthStencil = TRUE;			 //depth/stencil buffering

	if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D32, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D32;
	else if(GFXIsDepthFormatExisting(g_d3dcaps.DeviceType, D3DFMT_D16, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D16;

	//determine vertex processing...
	DWORD vertexFlag=D3DCREATE_HARDWARE_VERTEXPROCESSING;//D3DCREATE_MIXED_VERTEXPROCESSING;//

	if( g_d3dcaps.VertexShaderVersion < D3DVS_VERSION(1,1) )
		vertexFlag=D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	memcpy(&g_d3dPresentParam, &d3dParam, sizeof(g_d3dPresentParam));

	//create the device interface
	if(_GFXCheckError(
		g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,			   //primary for now...
                          g_d3dcaps.DeviceType,				   //depends
                          hwnd,								   //the window
						  vertexFlag,
                          //D3DCREATE_SOFTWARE_VERTEXPROCESSING, //software for now...
						  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
						  //D3DCREATE_MIXED_VERTEXPROCESSING,
                          &g_d3dPresentParam,
                          &g_p3DDevice), true, "Error in GFXInit"))
						  return RETCODE_FAILURE;

	//get caps
	//g_p3DDevice->GetDeviceCaps(&g_d3dcaps);

	//create the world stack
	D3DXCreateMatrixStack(0, &g_pWrldStack);
	g_pWrldStack->LoadIdentity();

	//set light
	GFXEnableLight(false);
	LightSetAmbient(AMBIENT_DEFAULT);

	//set up projection
	GFXResize(g_displayMode.width, g_displayMode.height);

	_GFXSetDefaultStates();

	//create texture vertex buffer, for blitting texture
	_TextureInitVtxBuff();

	g_bDeviceLost = false;
	
	//that's all...
	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFXGetMode
// Purpose:	get the current mode
// Output:	pModeOut set
// Return:	none
/////////////////////////////////////
PUBLIC void GFXGetMode(gfxDisplayMode *pModeOut)
{
	memcpy(pModeOut, &g_displayMode, sizeof(g_displayMode));
}


/////////////////////////////////////
// Name:	GFXSetMode
// Purpose:	set the mode in fullscreen
//			(does not work in window
//			 mode)
//			refresh rate can equal 0
//			to use previous
// Output:	mode set
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFXSetMode(gfxDisplayMode *pMode)
{
	if(g_d3dPresentParam.Windowed == FALSE)
	{
		g_d3dPresentParam.BackBufferWidth   = pMode->width;
		g_d3dPresentParam.BackBufferHeight  = pMode->height;
		g_d3dPresentParam.BackBufferFormat  = _GFXGetD3dFormat(pMode);

		g_displayMode.width = pMode->width;
		g_displayMode.height = pMode->height;
		g_displayMode.bpp = pMode->bpp;

		if(pMode->refresh > 0)
		{
			g_displayMode.refresh = pMode->refresh;
			g_d3dPresentParam.FullScreen_RefreshRateInHz = pMode->refresh;
		}

		return GFXReset();
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFXReset
// Purpose:	resets the device and
//			resources
// Output:	stuff reset
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFXReset()
{
	//g_d3dPresentParam.hDeviceWindow = hwnd;

	if(g_d3dPresentParam.Windowed == TRUE)
	{
		D3DDISPLAYMODE		  d3dDisplayMode={0};
		//Get desktop display mode.
	    if(_GFXCheckError(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), true, "Error in GFXInit"))
			return RETCODE_FAILURE;

		g_d3dPresentParam.BackBufferFormat  = d3dDisplayMode.Format;
	}

	//invalidate stuff
	FontInvalidateAll();
	TextureInvalidateAll();
	//invalidate particles?

	GFX_ScreenDestroy();

	_TextureTermVtxBuff();

	//LightReset();

	//reset
	HRESULT hr = g_p3DDevice->Reset(&g_d3dPresentParam);

	_GFXCheckError(hr, true, "GFXReset");

	GFXResize(g_displayMode.width, g_displayMode.height);

	//set the default states
	_GFXSetDefaultStates();

	//create texture vertex buffer, for blitting texture
	_TextureInitVtxBuff();

	//restore stuff
	FontRestoreAll();
	TextureRestoreAll();
	//restore particles?

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFXCheckDeviceLost
// Purpose:	checks to see if device lost.
//			if so, try to reset.
//			CALL THIS BEFORE RENDERING!
// Output:	reset stuff
// Return:	true if device is still lost
//			so no rendering
/////////////////////////////////////
PUBLIC bool GFXCheckDeviceLost()
{
	HRESULT hr;

	if(g_bDeviceLost)
	{
		hr = g_p3DDevice->TestCooperativeLevel();

		if(hr == D3DERR_DEVICELOST) //let's try again later
			return true;

		//do we need to reset?
		if(hr == D3DERR_DEVICENOTRESET)
		{
			//reset
			GFXReset();
		}

		g_bDeviceLost = false;

		return false;
	}

	return false;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXDestroy()
{
	LightDestroyAll();

	GFX_ScreenDestroy();

	PARFXDestroyAll();
	_OBJDestroyAll();
	_MDLDestroyAll();
	_TextureDestroyAll();

	_TextureTermVtxBuff();

	if(g_pWrldStack)
	{ g_pWrldStack->Release(); g_pWrldStack=0;}
	if(g_p3DDevice)
	{g_p3DDevice->Release(); g_p3DDevice=0;}
	if(g_pD3D)
	{g_pD3D->Release(); g_pD3D=0;}

	GFXPageClear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXClear(int numRect, const D3DRECT* pRects, DWORD color)
{
	g_p3DDevice->Clear(numRect,
                       pRects,
                       D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, //what to clear
                       color, //the color
                       1.0f,  //depth buffer value to clear
                       0 );   //stencil buffer
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXClearDepth()
{
	g_p3DDevice->Clear(0,
                       0,
                       D3DCLEAR_ZBUFFER, //what to clear
                       0, //the color
                       1.0f,  //depth buffer value to clear
                       0 );   //stencil buffer
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXUpdateScene()
{
	//update models
	GFXUpdateModels();

	//update all object
	hOBJ thisObj;

	for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
	{
		thisObj = (hOBJ)(*i);

		OBJUpdateFrame(thisObj);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXDisplayScene()
{
	//display all object
	hOBJ thisObj;

	for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
	{
		thisObj = (hOBJ)(*i);

		OBJDisplay(thisObj, true);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXUpdate(const RECT* pSrcRect, const RECT* pDestRect, HWND hwndOverride)
{
	HRESULT hr = g_p3DDevice->Present(pSrcRect, pDestRect, hwndOverride, 0);

	if(hr == D3DERR_DEVICELOST) //uh oh...
		g_bDeviceLost = true;
	else
	{
		_GFXCheckError(hr, true, "Error in GFXUpdate");
		return RETCODE_FAILURE;
	}

#ifdef _DEBUG
	g_numCulled=0;
#endif

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFXBegin
// Purpose:	readies device to render
//			stuff.
// Output:	shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBegin()
{
	g_p3DDevice->BeginScene();
}

/////////////////////////////////////
// Name:	GFXEnd
// Purpose:	call this after finish
//			with rendering stuff
// Output:	more shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnd()
{
	g_p3DDevice->EndScene();
}

/////////////////////////////////////
// Name:	GFXBltModeEnable
// Purpose:	use this for blitting
//			textures on 2d raster
//			coord.
//			as well as billboarding
// Output:	projection is changed
//			disables depth buffer
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeEnable(DWORD flag)
{
	//begin paint
   g_pTxtSprite->Begin(0);

   //disable depth buffering
   /*if(TESTFLAGS(flag, BLT_PRIORITY))
   {
	   g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
   }
   else
   {
	   g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	   g_p3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
   }*/

   //disable light
 //  if(GFXIsLightEnabled())
//	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

   //disable culling
 //  g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

   //set flag
   SETFLAG(g_FLAGS, GFX_BLTMODE);
}

/////////////////////////////////////
// Name:	GFXIsBltMode
// Purpose:	checks if bltmode is enabled
// Output:	none
// Return:	if true
/////////////////////////////////////
PUBLIC bool GFXIsBltMode()
{
	return TESTFLAGS(g_FLAGS, GFX_BLTMODE);
}

/////////////////////////////////////
// Name:	GFXBltModeDisable
// Purpose:	disable blt mode
// Output:	changes projection
//			back to perspective
//			enables depth buffer again
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeDisable()
{
	//end paint
   g_pTxtSprite->End();

	//enable depth buffering
    //g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//g_p3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

   //enable light again, if flag is set
//   if(GFXIsLightEnabled())
//	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

   //clear flag
   CLEARFLAG(g_FLAGS, GFX_BLTMODE);

   //enable culling
//	g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

/////////////////////////////////////
// Name:	GFXGetScrnSize
// Purpose:	grabs size of screen
// Output:	none
// Return:	the scrn size
/////////////////////////////////////
PUBLIC SIZE GFXGetScrnSize()
{
	SIZE s;
	s.cx = g_displayMode.width;
	s.cy = g_displayMode.height;
	return s;
}

/////////////////////////////////////
// Name:	GFXUpdateClientRect
// Purpose:	update the client rect
//			(use this on WM_MOVE or
//			whenever the window is
//			changed
// Output:	client rect set
// Return:	none
/////////////////////////////////////
PUBLIC void GFXUpdateClientRect(RECT *pRect)
{
	if(pRect)
		memcpy(&g_CLIENTRECT, pRect, sizeof(g_CLIENTRECT));
}