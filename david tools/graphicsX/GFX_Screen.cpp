#include "gdix.h"
#include "gdix_i.h"

#define SCRN_TXT_W	256
#define SCRN_TXT_H	256

int					g_surfW, g_surfH;

GFXSURFACE * g_surface = 0;		//the buffer to be filled by the screen

vector< vector< hTXT > > g_scrnTxts;	//the screen textures

/////////////////////////////////////
// Name:	GFX_ScreenDestroy
// Purpose:	destroy the buffer screen
//			and textures
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void GFX_ScreenDestroy()
{
	if(g_surface)
	{
		g_surface->Release();
		g_surface = 0;
	}

	for(int i = 0; i < g_scrnTxts.size(); i++)
	{
		for(int j = 0; j < g_scrnTxts[i].size(); j++)
		{
			if(g_scrnTxts[i][j])
			{ TextureDestroy(&g_scrnTxts[i][j]); g_scrnTxts[i][j] = 0; }
		}
	}
}

/////////////////////////////////////
// Name:	GFX_ScreenCreate
// Purpose:	create the surface screen
//			and the textures
// Output:	initializes the surface
//			and textures
// Return:	none
/////////////////////////////////////
PROTECTED void GFX_ScreenCreate(DWORD w, DWORD h)
{
	GFX_ScreenDestroy();

	D3DDISPLAYMODE dm;
	dm.Width = dm.Height = 0;

	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

	g_surfW = w;
	g_surfH = h;

	char buff[MAXCHARBUFF]; sprintf(buff, "w:%d h:%d", g_surfW, g_surfH);

	//create the surface
	if(_GFXCheckError(g_p3DDevice->CreateOffscreenPlainSurface(g_surfW, g_surfH,
    D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &g_surface, 0), true, buff))//"GFX_ScreenCreate"))
	{ return; }

	//set the textures up
	char txtname[MAXCHARBUFF];

	g_scrnTxts.resize((GFXGetScrnSize().cy/SCRN_TXT_H)+1);

	for(int i = 0; i < g_scrnTxts.size(); i++)
	{
		g_scrnTxts[i].resize((GFXGetScrnSize().cx/SCRN_TXT_W)+1);

		for(int j = 0; j < g_scrnTxts[i].size(); j++)
		{
			sprintf(txtname, "scrnTXT_%d_%d", i, j);

			g_scrnTxts[i][j] = TextureCreateCustom(0, txtname,
								SCRN_TXT_W, SCRN_TXT_H, 3,
								0, BPP_32);

			if(!g_scrnTxts[i][j]) //that means something went wrong
			{ GFX_ScreenDestroy(); return; }
		}
	}
}

/////////////////////////////////////
// Name:	GFX_ScreenCapture
// Purpose:	take a snapshot of what
//			currently is on screen
// Output:	screen stuff filled
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFX_ScreenCapture()
{
	//copy the surface
	IDirect3DSwapChain9 *pSwapChain;
	HRESULT hr = g_p3DDevice->GetSwapChain(0, &pSwapChain);

	if(SUCCEEDED(hr) && pSwapChain)
	{
		D3DDISPLAYMODE ddisp;
		pSwapChain->GetDisplayMode(&ddisp);

		D3DSURFACE_DESC ddesc;
		g_surface->GetDesc(&ddesc);

		if(_GFXCheckError(g_p3DDevice->GetFrontBufferData(0,g_surface), true, "GFX_ScreenCapture"))
			return RETCODE_FAILURE;

		//lock the surface
		D3DLOCKED_RECT surfRect;

		if(_GFXCheckError(g_surface->LockRect(&surfRect, 0, D3DLOCK_READONLY), true, "GFX_ScreenCapture"))
			return RETCODE_FAILURE;

		//now fill up the textures
		for(int i = 0; i < g_scrnTxts.size(); i++)
		{
			for(int j = 0; j < g_scrnTxts[i].size(); j++)
			{
				assert(g_scrnTxts[i][j]); //this should not happen...

				int xStart = j*SCRN_TXT_W + g_CLIENTRECT.left;
				int yStart = i*SCRN_TXT_H + g_CLIENTRECT.top;

				int xMax = xStart + SCRN_TXT_W;
				int yMax = yStart + SCRN_TXT_H;

				if(xMax > g_surfW)
					xMax = g_surfW;

				if(yMax > g_surfH)
					yMax = g_surfH;

				int pitch;
				BYTE *pTxtBits, *thisBit, *surfBit;

				if(TextureLock(g_scrnTxts[i][j], 0, 0, 0, &pitch, (void **)&pTxtBits) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0, "Failed to lock a texture!", "GFX_ScreenCapture"); return RETCODE_FAILURE; }

				//now fill 'er up
				for(int y = yStart, yTxt = 0; y < yMax; y++, yTxt++)
				{
					surfBit = ((BYTE*)surfRect.pBits) + (y*surfRect.Pitch);
					surfBit += xStart*4;

					thisBit = pTxtBits + (yTxt*pitch);

					for(int x = xStart, xTxt = 0; x < xMax; xTxt++, x++)
					{
						thisBit[3] = 255;//surfBit[3];
						thisBit[2] = surfBit[2];
						thisBit[1] = surfBit[1];
						thisBit[0] = surfBit[0];

						thisBit+=4;
						surfBit+=4;
					}
				}

				TextureUnlock(g_scrnTxts[i][j], 0);
			}
		}

		g_surface->UnlockRect();

		pSwapChain->Release();
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFX_ScreenDisplay
// Purpose:	display the snapshot of
//			the screen
//			NOTE: call GFXBltModeEnable
//				  first!!!
// Output:	stuff displayed
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFX_ScreenDisplay(const DWORD *color, float rotate)
{
	for(int i = 0; i < g_scrnTxts.size(); i++)
	{
		for(int j = 0; j < g_scrnTxts[i].size(); j++)
		{
			if(TextureBlt(g_scrnTxts[i][j], j*SCRN_TXT_W, i*SCRN_TXT_H, 0, color, rotate) != RETCODE_SUCCESS)
				return RETCODE_FAILURE;
		}
	}

	return RETCODE_SUCCESS;
}