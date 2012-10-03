#include "gdix.h"
#include "gdix_i.h"

/////////////////////////////////////
// Name:	TextureGetSurface
// Purpose:	get the surface of texture
//			destroy it after done using
// Output:	none
// Return:	the surface
/////////////////////////////////////
PUBLIC GFXSURFACE *TextureGetSurface(hTXT txt)
{
	GFXSURFACE *pSurf = 0;

	_GFXCheckError(txt->texture->GetSurfaceLevel(0, &pSurf), 
		true, "TextureGetSurface");

	return pSurf;
}

/////////////////////////////////////
// Name:	SurfaceDestroy
// Purpose:	destroy surface
// Output:	surface destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void SurfaceDestroy(GFXSURFACE *pSurf)
{
	if(pSurf)
	{
		pSurf->Release();
	}
}