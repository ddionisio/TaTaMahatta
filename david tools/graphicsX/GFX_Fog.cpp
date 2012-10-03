#include "gdix.h"
#include "gdix_i.h"

bool g_bCanDoTableFog=false;
bool g_bCanDoVertexFog=false;

/////////////////////////////////////
// Name:	GFXEnableFog
// Purpose:	enable/disable the fog
// Output:	fog enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnableFog(bool bYes)
{
	if(bYes)
	{
		g_p3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

		//determine type of fog we are going to use,
		//if we can do table fog, use that
		DWORD dwCaps = g_d3dcaps.RasterCaps;

		bool g_bCanDoTableFog  = (dwCaps&D3DPRASTERCAPS_FOGTABLE) &&
			((dwCaps&D3DPRASTERCAPS_ZFOG) || (dwCaps&D3DPRASTERCAPS_WFOG))
																   ? true : false;
		bool g_bCanDoVertexFog = (dwCaps&D3DPRASTERCAPS_FOGVERTEX) ? true : false;
	}
	else
	{
		g_p3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

/////////////////////////////////////
// Name:	GFXIsFogEnable
// Purpose:	check to see if fog is
//			enabled
// Output:	none
// Return:	true if enabled
/////////////////////////////////////
PUBLIC bool GFXIsFogEnable()
{
	DWORD state;

	g_p3DDevice->GetRenderState(D3DRS_FOGENABLE, &state);

	return (state == TRUE);
}

/////////////////////////////////////
// Name:	
// Purpose:	set mode to:
//			 D3DFOG_NONE
//			 D3DFOG_EXP    (uses density)
//			 D3DFOG_EXP2   (uses density)
//			 D3DFOG_LINEAR (uses start/end)
//			note: vertex fog can only
//				  use linear
// Output:	mode set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetMode(DWORD mode)
{
	if(g_bCanDoTableFog)
	{
		g_p3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
        g_p3DDevice->SetRenderState( D3DRS_FOGTABLEMODE,   mode );

		g_p3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
	}
	else if(g_bCanDoVertexFog)
	{
		g_p3DDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
        g_p3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  mode );

		g_p3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );
	}
}

/////////////////////////////////////
// Name:	FogSetColor
// Purpose:	set color of fog
// Output:	color of fog is set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetColor(DWORD clr)
{
	g_p3DDevice->SetRenderState( D3DRS_FOGCOLOR,  clr );
}

/////////////////////////////////////
// Name:	FogSetStart
// Purpose:	set the start of fog,
//			used in linear mode
// Output:	fog start set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetStart(float val)
{
	g_p3DDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(val) );
}

/////////////////////////////////////
// Name:	FogSetEnd
// Purpose:	set the end of fog,
//			used in linear mode
// Output:	fog end set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetEnd(float val)
{
	g_p3DDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(val) );
}

/////////////////////////////////////
// Name:	FogSetDensity
// Purpose:	set the density of fog,
//			used in exp/exp2 mode
// Output:	fog density set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetDensity(float val)
{
	g_p3DDevice->SetRenderState( D3DRS_FOGDENSITY, FtoDW(val) );
}