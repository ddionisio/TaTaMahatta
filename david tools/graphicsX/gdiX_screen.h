#ifndef _gdix_screen_h
#define _gdix_screen_h

//THIS IS FOR INTERNAL GFX USE ONLY!

/////////////////////////////////////
// Name:	GFX_ScreenDestroy
// Purpose:	destroy the buffer screen
//			and textures
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void GFX_ScreenDestroy();

/////////////////////////////////////
// Name:	GFX_ScreenCreate
// Purpose:	create the surface screen
//			and the textures
// Output:	initializes the surface
//			and textures
// Return:	none
/////////////////////////////////////
PROTECTED void GFX_ScreenCreate(DWORD w, DWORD h);

#endif