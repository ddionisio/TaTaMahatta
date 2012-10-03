#include "gdix_bkgrndfx.h"
#include "gdix_i.h"

#define MAX_ALPHA	255

//message for FX functions
typedef enum {
	BKFXM_CREATE,
	BKFXM_UPDATE,
	BKFXM_DISPLAY,
	BKFXM_DESTROY
} FXmessage;

//signals for FX update
typedef enum {
	BKFXsignal_animate,
	BKFXsignal_noanimate
} BKFXsignal;

/**********************************************************
***********************************************************

  Data structs for the background FXs

***********************************************************
**********************************************************/

//
// background FX main data struct for each type
//

#define GFXCLRVTX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE) //xyzw and color

typedef struct _gfxClrVtx {
    float x, y, z, rhw;
    DWORD color;
} gfxClrVtx;

//fade in/out
typedef struct _fadeinout {
	BYTE		r,g,b;				//the color
	int			curA;				//current alpha
	bool		bFadeIn;			//are we fading to the color?
	win32Time	timer;				//the timer
	GFXVTXBUFF  *vtx;	//four point vtx
} fadeinout;

typedef struct _imgfadeinout {
	hTXT theImg;	//image to transit to
	int curA;		//current alpha
	bool		bFadeIn;			//are we fading to the color?
	win32Time	timer;				//the timer
} imgfadeinout;

//curtain
typedef struct _curtain {
	hTXT curtainImg;		//curtain to fall down
	unsigned int fadeColor;	//fadeout color after curtain goes down
} curtain;

//fade away
/*
typedef struct _fadeaway_init {
	BYTE r,g,b;
	double delay;
} curtain_init;*/
typedef struct _fadeaway {
	BYTE rStart,gStart,bStart;
	BYTE rEnd,gEnd,bEnd;

	int r,g,b;		//current r/g/b

	int curA;		//current alpha

	win32Time	timer;				//the timer
} fadeaway;

/**********************************************************
***********************************************************

  The methods for all FX

***********************************************************
**********************************************************/

//fade in/out
PROTECTED RETCODE bkFadeInOutFunc(hBKFX bkFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fadeinout *thisData = (fadeinout *)bkFX->Data;

	switch(message)
	{
	case BKFXM_UPDATE:
		{
			double t;

			t = TimeGetTime(&thisData->timer)/TimeGetDelay(&thisData->timer);
			
			if(thisData->bFadeIn)
				thisData->curA = t*MAX_ALPHA;
			else
				thisData->curA = MAX_ALPHA - (t*MAX_ALPHA);

			if(thisData->curA > MAX_ALPHA) thisData->curA = MAX_ALPHA;
			else if(thisData->curA < 0) thisData->curA = 0;

			gfxClrVtx *vtxData;

			DWORD clr = D3DCOLOR_RGBA(thisData->r, thisData->g, thisData->b, thisData->curA);

			if(SUCCEEDED(thisData->vtx->Lock(0,0,(void**)&vtxData, 0)))
			{
				vtxData[0].color = clr;
				vtxData[1].color = clr;
				vtxData[2].color = clr;
				vtxData[3].color = clr;

				thisData->vtx->Unlock();
			}

			if(t >= 1)
			{
				TimeReset(&thisData->timer);

				if(thisData->curA == MAX_ALPHA) //now fade out
					thisData->bFadeIn = false;
				else if(thisData->curA == 0)	//check to see if we are over
					return RETCODE_BREAK;
			}
		}
		break;

	case BKFXM_DISPLAY:
		if(thisData->bFadeIn)
		{
			GFXBltModeEnable(0);
			GFX_ScreenDisplay(0, 0);
			GFXBltModeDisable();
		}

		g_p3DDevice->SetFVF(GFXCLRVTX);

		g_p3DDevice->SetStreamSource(0, thisData->vtx, 0, sizeof(gfxClrVtx));

		g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

		g_p3DDevice->SetStreamSource(0,0,0,0);

		//GFXBltModeDisable();
		break;

	case BKFXM_CREATE:
		{
			fadeinout_init *initData = (fadeinout_init *)dumbParam;

			if(!initData)
				return RETCODE_FAILURE;

			fadeinout *newData = (fadeinout *)GFX_MALLOC(sizeof(fadeinout));

			if(!newData)
				return RETCODE_FAILURE;

			newData->r = initData->r; 
			newData->g = initData->g;
			newData->b = initData->b;

			//start at alpha = 0 and bFadeIn = true
			newData->curA    = 0;
			newData->bFadeIn = true;

			//set timer
			TimeInit(&newData->timer, initData->delay);

			//create flat plane
			SIZE scrnSize = GFXGetScrnSize();

			if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(gfxClrVtx)*NUMPTQUAD,
				D3DUSAGE_WRITEONLY, GFXCLRVTX, D3DPOOL_MANAGED, &newData->vtx, 0),
				true, "bkFadeInOutFunc"))
					return RETCODE_FAILURE;

			gfxClrVtx *vtxData;

			DWORD clr = D3DCOLOR_RGBA(newData->r, newData->g, newData->b, newData->curA);

			if(SUCCEEDED(newData->vtx->Lock(0,0,(void**)&vtxData, 0)))
			{
				vtxData[0].x = 0; vtxData[0].y = 0; vtxData[0].z = 0; vtxData[0].rhw = 1;
				vtxData[0].color = clr;

				vtxData[1].x = 0; vtxData[1].y = scrnSize.cy; vtxData[1].z = 0; vtxData[1].rhw = 1;
				vtxData[1].color = clr;

				vtxData[2].x = scrnSize.cx; vtxData[2].y = scrnSize.cy; vtxData[2].z = 0; vtxData[2].rhw = 1;
				vtxData[2].color = clr;

				vtxData[3].x = scrnSize.cx; vtxData[3].y = 0; vtxData[3].z = 0; vtxData[3].rhw = 1;
				vtxData[3].color = clr;

				newData->vtx->Unlock();
			}

			bkFX->Data = newData;

			//get a snap-shot of the screen
			GFX_ScreenCapture();
		}
		break;

	case BKFXM_DESTROY:
		{
			if(thisData)
				if(thisData->vtx)
					thisData->vtx->Release();
		}
		break;
	}

	return RETCODE_SUCCESS;
}

//image fade in/out
PROTECTED RETCODE bkImgFadeInOutFunc(hBKFX bkFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	imgfadeinout *thisData = (imgfadeinout *)bkFX->Data;

	switch(message)
	{
	case BKFXM_UPDATE:
		{
			double t;

			t = TimeGetTime(&thisData->timer)/TimeGetDelay(&thisData->timer);
			
			if(thisData->bFadeIn)
				thisData->curA = t*MAX_ALPHA;
			else
				thisData->curA = MAX_ALPHA - (t*MAX_ALPHA);

			if(thisData->curA > MAX_ALPHA) thisData->curA = MAX_ALPHA;
			else if(thisData->curA < 0) thisData->curA = 0;

			if(t >= 1)
			{
				TimeReset(&thisData->timer);

				if(thisData->curA == MAX_ALPHA) //now fade out
					thisData->bFadeIn = false;
				else if(thisData->curA == 0)	//check to see if we are over
					return RETCODE_BREAK;
			}
		}
		break;

	case BKFXM_DISPLAY:
		{
			SIZE sSize = GFXGetScrnSize();
			DWORD clr = D3DCOLOR_RGBA(255, 255, 255, thisData->curA);

			GFXBltModeEnable(0);
			
			if(thisData->bFadeIn)
				GFX_ScreenDisplay(0, 0);

			TextureStretchBlt(thisData->theImg, 0, 0, sSize.cx,sSize.cy,0,&clr,0);

			GFXBltModeDisable();
		}
		break;

	case BKFXM_CREATE:
		{
			imgfadeinout_init *initData = (imgfadeinout_init *)dumbParam;

			if(!initData)
				return RETCODE_FAILURE;

			imgfadeinout *newData = (imgfadeinout *)GFX_MALLOC(sizeof(imgfadeinout));

			if(!newData)
				return RETCODE_FAILURE;

			//start at alpha = 0 and bFadeIn = true
			newData->curA    = 0;
			newData->bFadeIn = true;

			//set timer
			TimeInit(&newData->timer, initData->delay);

			//add ref to texture
			newData->theImg = initData->theImg;
			TextureAddRef(newData->theImg);

			bkFX->Data = newData;

			//get a snap-shot of the screen
			GFX_ScreenCapture();
		}
		break;

	case BKFXM_DESTROY:
		if(thisData)
			TextureDestroy(&thisData->theImg);
		break;
	}

	return RETCODE_SUCCESS;
}

//fade away
PROTECTED RETCODE bkFadeAwayFunc(hBKFX bkFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	fadeaway *thisData = (fadeaway *)bkFX->Data;

	switch(message)
	{
	case BKFXM_UPDATE:
		{
			double t;

			t = TimeGetTime(&thisData->timer)/TimeGetDelay(&thisData->timer);
			
			thisData->curA = MAX_ALPHA - (t*MAX_ALPHA);

			if(thisData->curA < 0) thisData->curA = 0;

			thisData->r = thisData->rStart + t*(thisData->rEnd-thisData->rStart);
			thisData->g = thisData->gStart + t*(thisData->gEnd-thisData->gStart);
			thisData->b = thisData->bStart + t*(thisData->bEnd-thisData->bStart);

			if(t >= 1)
			{
				TimeReset(&thisData->timer);

				if(thisData->curA == 0)	//check to see if we are over
					return RETCODE_BREAK;
			}
		}
		break;

	case BKFXM_DISPLAY:
		{
			DWORD clr = D3DCOLOR_RGBA(thisData->r,thisData->g,thisData->b,thisData->curA);

			GFXBltModeEnable(0);
			
			GFX_ScreenDisplay(&clr, 0);

			GFXBltModeDisable();
		}
		break;

	case BKFXM_CREATE:
		{
			fadeaway_init *initData = (fadeaway_init *)dumbParam;

			if(!initData)
				return RETCODE_FAILURE;

			fadeaway *newData = (fadeaway *)GFX_MALLOC(sizeof(fadeaway));

			if(!newData)
				return RETCODE_FAILURE;

			//start at alpha = 255
			newData->curA    = MAX_ALPHA;
			
			newData->rEnd = initData->r;
			newData->gEnd = initData->g;
			newData->bEnd = initData->b;

			newData->rStart = newData->gStart = newData->bStart = 255;

			bkFX->Data = newData;

			//get a snap-shot of the screen
			GFX_ScreenCapture();

			//set timer
			TimeInit(&newData->timer, initData->delay);
		}
		break;

	case BKFXM_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//
// A bunch of FX functions
//
PRIVATE BKEFFECT BKEffectTable[eBKFX_MAX] = {bkFadeInOutFunc,bkImgFadeInOutFunc,bkFadeAwayFunc};

/////////////////////////////////////
// Name:	BKFXDestroy
// Purpose:	destroys given bkFX
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void BKFXDestroy(hBKFX *bkFX)
{
	if(*bkFX)
	{
		if((*bkFX)->Effect)
			(*bkFX)->Effect(*bkFX, BKFXM_DESTROY, 0, 0);

		if((*bkFX)->Data)
			GFX_FREE((*bkFX)->Data);

		GFX_FREE(*bkFX);
	}

	bkFX=0;
}

/////////////////////////////////////
// Name:	BKFXCreate
// Purpose:	creates a background FX
// Output:	none
// Return:	the new background FX
/////////////////////////////////////
PUBLIC hBKFX BKFXCreate(eBKFXType type, void *initStruct)
{
	hBKFX newBkFX = (hBKFX)GFX_MALLOC(sizeof(gfxBkFX));

	if(!newBkFX) { ASSERT_MSG(0, "Unable to allocate new background FX", "BKFXCreate"); return 0; }

	newBkFX->Type = type;

	newBkFX->Effect = BKEffectTable[type];

	if(newBkFX->Effect(newBkFX, BKFXM_CREATE, (LPARAM)initStruct, 0) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new background FX", "BKFXCreate"); BKFXDestroy(&newBkFX); return 0; }

	return newBkFX;
}

/////////////////////////////////////
// Name:	BKFXUpdate
// Purpose:	updates and displays the
//			background FX
// Output:	stuff displayed
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE BKFXUpdate(hBKFX bkFX)
{
	return bkFX->Effect(bkFX, BKFXM_UPDATE, 0, 0);
}

/////////////////////////////////////
// Name:	BKFXDisplay
// Purpose:	display background FX
// Output:	stuff
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE BKFXDisplay(hBKFX bkFX)
{
	return bkFX->Effect(bkFX, BKFXM_DISPLAY, 0, 0);
}