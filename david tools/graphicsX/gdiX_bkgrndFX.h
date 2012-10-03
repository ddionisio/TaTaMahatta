#ifndef _gdix_bkgrndfx_h
#define _gdix_bkgrndfx_h

#include "gdix.h"

typedef struct _gfxBkFX		*hBKFX;	//handle to background FX

typedef enum {
	eBKFX_FADEINOUT,	//in  - fade from color to background
						//out - fade from background to color
	eBKFX_IMGFADEINOUT,	//same as above...except transition would be an image
	eBKFX_FADEAWAY,		//fade the old scene away
	eBKFX_MAX
} eBKFXType;

/**********************************************************
***********************************************************

  The init structs you must fill up for FX

***********************************************************
**********************************************************/

//
// background FX init struct for each type
//
typedef struct _fadeinout_init {
	BYTE r,g,b;
	double delay;
} fadeinout_init;

typedef struct _imgfadeinout_init {
	hTXT theImg;	//image to transit to
	double delay;
} imgfadeinout_init;

typedef struct _fadeaway_init {
	BYTE r,g,b;
	double delay;
} fadeaway_init;

/////////////////////////////////////
// Name:	BKFXCreate
// Purpose:	creates a background FX
// Output:	none
// Return:	the new background FX
/////////////////////////////////////
PUBLIC hBKFX BKFXCreate(eBKFXType type, void *initStruct);

/////////////////////////////////////
// Name:	BKFXUpdate
// Purpose:	updates and displays the
//			background FX
// Output:	stuff displayed
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE BKFXUpdate(hBKFX bkFX);

/////////////////////////////////////
// Name:	BKFXDisplay
// Purpose:	display background FX
// Output:	stuff
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE BKFXDisplay(hBKFX bkFX);

/////////////////////////////////////
// Name:	BKFXDestroy
// Purpose:	destroys given bkFX
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void BKFXDestroy(hBKFX *bkFX);

#endif