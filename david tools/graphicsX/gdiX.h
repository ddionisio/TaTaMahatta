#ifndef _gdix_h
#define _gdix_h

#include "..\common.h"

#include "d3d9types.h"
#include "d3dx9math.h"

#include "d3dstuff\D3DFont.h"

#ifdef _DEBUG
extern int g_numCulled;
extern int g_numFaceProcessed;
#endif

//NOTES:
// 1. We are using right-handed coord
// 2. Z-axis is the depth
// 3. vertices are drawn in CW order


//public constants
#define NUMPTFACE		3	//well duh?
#define NUMPTQUAD		4
#define NUMMTXELEM		16

#define NUMVTXTEXTURE	3	//max number of texture coords per vertex

#define DAVIDPI			3.1415926535897932384626433832795f

#define INFINITE_DURATION -1

//public flags

//fonts
#define FNT_BOLD        1
#define FNT_ITALIC      2

//blit mode
#define BLT_PRIORITY	1

//vector aabb returns
#define BBOX_IN			1
#define BBOX_OUT		0
#define BBOX_INTERSECT	2

//public enum
typedef enum {
	BPP_32,	 //32-bit
	BPP_24,	 //24-bit
	BPP_16,  //16-bit r5 g6 b5
	BPP_15a, //16-bit r5 g5 b5 a1
	BPP_15,  //15-bit r5 g5 b5
	BPP_8,	 //8-bit color index
	BPP_DOH, //d'oh!
} eBPP;

typedef enum {
	eR,
	eG,
	eB,
	eA,
	eMaxClr
} eColor;

typedef enum {
	eX,
	eY,
	eZ,
	eMaxPt
} ePt;

typedef enum {
	eU,
	eV,
	eMaxTxtPt
} eTxtPt;

typedef enum {
	e_11=0, e_21=4, e_31=8,  e_41=12,
    e_12=1, e_22=5, e_32=9,  e_42=13,
    e_13=2, e_23=6, e_33=10, e_43=14,
    e_14=3, e_24=7, e_34=11, e_44=15,
	eMaxMtxElem=16
} eMtx;

typedef enum {
	ePA,
	ePB,
	ePC,
	ePD,
	eMaxPlaneEq
} ePlane;

typedef enum {
	eOBJ_STILL,
	eOBJ_ANIMATE
} eOBJTYPE;

//public typedefs
typedef int	   angle;

//wrapper typedef so that we can upgrade easily
typedef IDirect3DSurface9		GFXSURFACE;
typedef D3DMATERIAL9			GFXMATERIAL;
typedef D3DLIGHT9				GFXLIGHT;

//public handles
typedef struct _gfxCamera  *hCAMERA;
typedef struct _gfxTexture *hTXT;
typedef struct _gfxModel   *hMDL;
typedef struct _gfxObject  *hOBJ;
typedef struct _gfxFont	   *hFNT;
typedef struct _gfxSprite  *hSPRITE;
typedef struct _gfxTMap	   *hTMAP;
typedef struct _gfxQBSP	   *hQBSP;

//public structs
typedef struct _gfxDisplayMode {
	unsigned int width;		//scrn width
	unsigned int height;	//scrn height
	eBPP		 bpp;		//bits per pixel
	unsigned int refresh;	//refresh rate (usu. 0)
} gfxDisplayMode;


typedef struct _gfxVtx {
    float x, y, z; //model space vertex position
	float nX, nY, nZ; //normal for lighting
	float txt[NUMVTXTEXTURE][eMaxTxtPt];
} gfxVtx;

typedef struct _gfxSprFrame {
	int firstframe; //Index of first frame
	int lastframe;	//Index of last frame
	int curFrame;	//Index of current frame
} gfxSprFrame;

//bounding stuff for model
typedef struct _gfxBound {
	float center[eMaxPt];		   //center of bounding sphere
	float radius;				   //the radius
	float min[eMaxPt],max[eMaxPt]; //bounding box min/max
} gfxBound;

//ID for objects
typedef struct _gfxID {
	DWORD ID;
	LONGLONG counter;
} gfxID;

//GFX QBSP collision data
typedef struct _gfxTrace {
	int		txtInd;					 //texture index in QBSP
	bool	bAllSolid;				 //true if plane is not valid
	bool	bStartSolid;			 //true if initial point was in solid area
	float	t;						 //time parameter (1.0 = no hit)
	float	norm[eMaxPt];			 //the normal of plane
	float	d;						 //the plane element 'd', plane distance from origin

	int		leafIndex;				 //the leaf index of collision
	int		brushIndex;				 //the brush index of collision (-1 for none)
} gfxTrace;

//parse an entity from QBSP map
typedef void (* ENTITYPARSEFUNC) (char *entityStr, hQBSP qbsp, void *userDat);

//
//GFX main functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXResize(unsigned int width, unsigned int height);

/////////////////////////////////////
// Name:	GFXSetTextureStageState/
//			GFXGetTextureStageState
// Purpose:	set/get texture stage state
// Output:	val filled if get
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE GFXSetTextureStageState(DWORD base,
									   DWORD type,
									   DWORD val);

PUBLIC void GFXGetTextureStageState(DWORD base,
									DWORD type,
									DWORD *val);

/////////////////////////////////////
// Name:	GFXSetSampleState/
//			GFXGetSampleState
// Purpose:	set/get sample state
// Output:	val filled if get
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE GFXSetSampleState(DWORD sampler,
								 DWORD type,
								 DWORD val);

PUBLIC void GFXGetSampleState(DWORD sampler,
							  DWORD type,
							  DWORD *val);

/////////////////////////////////////
// Name:	GFXSetRenderState/
//			GFXGetRenderState
// Purpose:	set/get sample state
// Output:	val filled if get
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE GFXSetRenderState(DWORD state, DWORD val);
PUBLIC void GFXGetRenderState(DWORD state, DWORD *val);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXInit(HWND hwnd, gfxDisplayMode *pMode);

/////////////////////////////////////
// Name:	GFXGetMode
// Purpose:	get the current mode
// Output:	pModeOut set
// Return:	none
/////////////////////////////////////
PUBLIC void GFXGetMode(gfxDisplayMode *pModeOut);

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
PUBLIC RETCODE GFXSetMode(gfxDisplayMode *pMode);

/////////////////////////////////////
// Name:	GFXReset
// Purpose:	resets the device and
//			resources
// Output:	stuff reset
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFXReset();

/////////////////////////////////////
// Name:	GFXCheckDeviceLost
// Purpose:	checks to see if device lost.
//			if so, try to reset.
//			CALL THIS BEFORE RENDERING!
// Output:	reset stuff
// Return:	true if device is still lost
//			so no rendering
/////////////////////////////////////
PUBLIC bool GFXCheckDeviceLost();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXDestroy();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXClear(int numRect, const D3DRECT* pRects, DWORD color);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXClearDepth();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXUpdateScene();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXDisplayScene();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXUpdate(const RECT* pSrcRect, const RECT* pDestRect, HWND hwndOverride);

/////////////////////////////////////
// Name:	GFXBegin
// Purpose:	readies device to render
//			stuff.
// Output:	shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBegin();

/////////////////////////////////////
// Name:	GFXEnd
// Purpose:	call this after finish
//			with rendering stuff
// Output:	more shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnd();

/////////////////////////////////////
// Name:	GFXBltModeEnable
// Purpose:	use this for blitting
//			textures on 2d raster
//			coord.
//			as well as billboarding
//			for flags use:
//			BLT_PRIORITY...etc.
// Output:	projection is changed
//			disables depth buffer
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeEnable(DWORD flag);

/////////////////////////////////////
// Name:	GFXIsBltMode
// Purpose:	checks if bltmode is enabled
// Output:	none
// Return:	if true
/////////////////////////////////////
PUBLIC bool GFXIsBltMode();

/////////////////////////////////////
// Name:	GFXBltModeDisable
// Purpose:	disable blt mode
// Output:	changes projection
//			back to perspective
//			enables depth buffer again
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeDisable();

/////////////////////////////////////
// Name:	GFXGetScrnSize
// Purpose:	grabs size of screen
// Output:	none
// Return:	the scrn size
/////////////////////////////////////
PUBLIC SIZE GFXGetScrnSize();

/////////////////////////////////////
// Name:	GFXUpdateModels
// Purpose:	update all models (used for Model FXs)
// Output:	stuff updated
// Return:	none
/////////////////////////////////////
PUBLIC void GFXUpdateModels();

/////////////////////////////////////
// Name:	GFXUpdateClientRect
// Purpose:	update the client rect
//			(use this on WM_MOVE or
//			whenever the window is
//			changed
// Output:	client rect set
// Return:	none
/////////////////////////////////////
PUBLIC void GFXUpdateClientRect(RECT *pRect);

//
//camera functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hCAMERA CameraCreate(const float vEye[eMaxPt], angle yaw, angle pitch, angle roll);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraSetScene(hCAMERA cam);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraMov(hCAMERA cam, const float amt[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraSet(hCAMERA cam, const float vEye[eMaxPt]);

/////////////////////////////////////
// Name:	CameraSetTarget
// Purpose:	set camera target
// Output:	set the camera target
// Return:	none
/////////////////////////////////////
PUBLIC void CameraSetTarget(hCAMERA cam, const float vTarget[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraMovDir(hCAMERA cam, float xAmt, float yAmt, float zAmt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraYawPitchRoll(hCAMERA cam, angle yaw, angle pitch, angle roll);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraDestroy(hCAMERA *cam);

/////////////////////////////////////
// Name:	CameraGetLoc
// Purpose:	grabs camera's eye loc
// Output:	the eye location
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetLoc(hCAMERA cam, float loc[eMaxPt]);

/////////////////////////////////////
// Name:	CameraGetDir
// Purpose:	grabs camera's direction
//			vector
// Output:	the direction vector
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetDir(hCAMERA cam, float dir[eMaxPt]);

/////////////////////////////////////
// Name:	CameraGetYawPitchRoll
// Purpose:	grabs camera's yaw/pitch/roll
// Output:	none
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetYawPitchRoll(hCAMERA cam, angle *yaw, angle *pitch, angle *roll);

//
//texture functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreateCustom(unsigned int newID, const char *name,
								DWORD width, DWORD height, DWORD mipLevels,
								DWORD usage, eBPP bpp);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreate(unsigned int newID, const char *filename, bool clrkey, D3DCOLOR srcClrKey);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureAddRef(hTXT txt);

/////////////////////////////////////
// Name:	TextureSetListFile
// Purpose:	sets the texture list file
// Output:	the texture list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSetListFile(const char *filename);

/////////////////////////////////////
// Name:	TextureSearchName
// Purpose:	get texture by it's name
// Output:	none
// Return:	the texture if found
/////////////////////////////////////
PUBLIC hTXT TextureSearchName(const char *name);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureSearch(unsigned int ID);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureDestroy(hTXT *txt);

/////////////////////////////////////
// Name:	TextureSet
// Purpose:	sets texture for texture
//			mapping.  Base is usually
//			0.
// Output:	texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSet(hTXT txt, unsigned int base);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt(hTXT txt, float x, float y, const RECT *srcRect,
								 const DWORD *color, float rotate);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureStretchBlt(hTXT txt, float x, float y, 
								 float w, float h, const RECT *srcRect,
								 const DWORD *color, float rotate);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt3D(hTXT txt, const D3DXMATRIX *wrldMtx,
							float fClr[eMaxClr]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBltBillboard(hTXT txt, const float center[eMaxPt],
								   const float size, float fClr[eMaxClr]);

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
								   const GFXMATERIAL *material, const DWORD *color);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetWidth(hTXT txt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetHeight(hTXT txt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int TextureGetID(hTXT txt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureLock(hTXT txt, DWORD lvl, const RECT *pRect, DWORD flags,
						   int *pitchOut, void **bitsOut);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureUnlock(hTXT txt, DWORD lvl);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreateAlphaCircle(const char *name, 
									 BYTE r, BYTE g, BYTE b,
									 DWORD usage, DWORD radius);

/////////////////////////////////////
// Name:	TextureGetSurface
// Purpose:	get the surface of texture
//			destroy it after done using
// Output:	none
// Return:	the surface
/////////////////////////////////////
PUBLIC GFXSURFACE *TextureGetSurface(hTXT txt);

//
//surface functions
//

/////////////////////////////////////
// Name:	SurfaceDestroy
// Purpose:	destroy surface
// Output:	surface destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void SurfaceDestroy(GFXSURFACE *pSurf);

//
//model functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLCreate(unsigned int newID, const char *filename);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLGenMap(unsigned int newID, float mapSizeX, float mapSizeZ, float height, float r,
				 unsigned int numIter, hTXT texture, GFXMATERIAL *mat);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLAddRef(hMDL model);

/////////////////////////////////////
// Name:	MDLSetListFile
// Purpose:	sets the model list file
// Output:	the model list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLSetListFile(const char *filename);

/////////////////////////////////////
// Name:	MDLSearchName
// Purpose:	search model by name
// Output:	none
// Return:	the model
/////////////////////////////////////
PUBLIC hMDL MDLSearchName(const char *name);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLSearch(unsigned int ID);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLDestroy(hMDL *model);

/////////////////////////////////////
// Name:	MDLGetTexture
// Purpose:	get model's
//			texture within material
//			index
// Output:	none
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT MDLGetTexture(hMDL model, int materialInd);

/////////////////////////////////////
// Name:	MDLSetTexture
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetTexture(hMDL model, int materialInd, hTXT texture);

/////////////////////////////////////
// Name:	MDLSetMaterial
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetMaterial(hMDL model, int materialInd, const GFXMATERIAL &material);

/////////////////////////////////////
// Name:	MDLGetBound
// Purpose:	grabs model's bound info
// Output:	none
// Return:	the model's bound
/////////////////////////////////////
PUBLIC gfxBound MDLGetBound(hMDL model);

/////////////////////////////////////
// Name:	MDLGetNumVtx
// Purpose:	grabs the number of vtx
//			the model has
// Output:	none
// Return:	number of vtx
/////////////////////////////////////
PUBLIC DWORD MDLGetNumVtx(hMDL model);

/////////////////////////////////////
// Name:	MDLGetVtx
// Purpose:	grabs the vtx of model with
//			given number.
// Output:	none
// Return:	number of vtx copied
/////////////////////////////////////
PUBLIC DWORD MDLGetVtx(hMDL model, float vtx[100][eMaxPt], int numVtx);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int MDLGetID(hMDL model);

/////////////////////////////////////
// Name:	MDLGetNumGroup
// Purpose:	get number of group
// Output:	none
// Return:	number of group
/////////////////////////////////////
PUBLIC int MDLGetNumGroup(hMDL mdl);

/////////////////////////////////////
// Name:	MDLGetGroupName
// Purpose:	get name of group
// Output:	sOut filled, NULL if 
//			non-existence
// Return:	none
/////////////////////////////////////
PUBLIC void MDLGetGroupName(hMDL mdl, int index, char *sOut);

/////////////////////////////////////
// Name:	MDLGetGroupInd
// Purpose:	get group index
// Output:	none
// Return:	group index (-1 not found)
/////////////////////////////////////
PUBLIC int MDLGetGroupInd(hMDL mdl, const char *groupName);

/////////////////////////////////////////////
//Model FX Editor Purpose
/////////////////////////////////////////////

/////////////////////////////////////
// Name:	MDLFXDestroy
// Purpose:	destroys FX from a group
// Output:	FX destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXDestroy(hMDL mdl, int groupInd);

/////////////////////////////////////
// Name:	MDLFXInit
// Purpose:	initializes FX for group 
// Output:	FX initialize
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE MDLFXInit(hMDL mdl, int groupInd);

/////////////////////////////////////
// Name:	MDLFXAddRenderState
// Purpose:	add a render state to
//			FX
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXAddRenderState(hMDL mdl, int groupInd, DWORD state, DWORD val);

/////////////////////////////////////
// Name:	MDLFXAddTextureState
// Purpose:	add a texture state to FX
// Output:	texture state added
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXAddTextureState(hMDL mdl, int groupInd, 
									 DWORD txtStage, DWORD state, DWORD val);

/////////////////////////////////////
// Name:	MDLFXSetTextureAnimTrans
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTextureAnimTrans(hMDL mdl, int groupInd, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t);

/////////////////////////////////////
// Name:	MDLFXSetTextureAnimRot
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTextureAnimRot(hMDL mdl, int groupInd, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t);

/////////////////////////////////////
// Name:	MDLFXSetTextureAnimScale
// Purpose:	set texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTextureAnimScale(hMDL mdl, int groupInd, DWORD txtStage,
									float spd_s, float spd_t,
									float acc_s, float acc_t,
									float min_s, float min_t,
									float max_s, float max_t);

/////////////////////////////////////
// Name:	MDLFXGetTextureAnimTrans
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MDLFXGetTextureAnimTrans(hMDL mdl, int groupInd, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t);

/////////////////////////////////////
// Name:	MDLFXGetTextureAnimRot
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MDLFXGetTextureAnimRot(hMDL mdl, int groupInd, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t);

/////////////////////////////////////
// Name:	MDLFXGetTextureAnimScale
// Purpose:	get texture animation
// Output:	texture animation set
// Return:	none
/////////////////////////////////////
PROTECTED void MDLFXGetTextureAnimScale(hMDL mdl, int groupInd, DWORD txtStage,
									float *spd_s, float *spd_t,
									float *acc_s, float *acc_t,
									float *min_s, float *min_t,
									float *max_s, float *max_t);

/////////////////////////////////////
// Name:	MDLFXEnableTextureAnim
// Purpose:	enable/disable texture anim
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXEnableTextureAnim(hMDL mdl, int groupInd, DWORD txtStage, bool bEnable);

/////////////////////////////////////
// Name:	MDLFXIsTextureAnim
// Purpose:	is texture anim enable?
// Output:	none
// Return:	true if so
/////////////////////////////////////
PROTECTED bool MDLFXIsTextureAnim(hMDL mdl, int groupInd, DWORD txtStage);

/////////////////////////////////////
// Name:	MDLFXSetTexture
// Purpose:	set the texture to apply
//			give txt = 0 to unset the
//			texture
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void MDLFXSetTexture(hMDL mdl, int groupInd, DWORD txtStage, hTXT txt);

/////////////////////////////////////
// Name:	MDLFXGetRenderState
// Purpose:	get the render state value
//			if it exists
// Output:	valOut is filled
// Return:	true if state exists
/////////////////////////////////////
PUBLIC bool MDLFXGetRenderState(hMDL mdl, int groupInd, DWORD state, DWORD *valOut);

/////////////////////////////////////
// Name:	MDLFXGetTextureState
// Purpose:	get the texture state value
//			if it exists
// Output:	valOut is filled
// Return:	true if state exists
/////////////////////////////////////
PUBLIC bool MDLFXGetTextureState(hMDL mdl, int groupInd, DWORD txtStage, 
									DWORD state, DWORD *valOut);

/////////////////////////////////////
// Name:	MDLFXLoad
// Purpose:	load FX for the model
//			NOTE: texture paths use
//			relative path to filename
//			NOTE: all groups inside
//			the FX must correspond to
//			those found inside the model.
//			NOTE: this function is automatically
//			called when creating the model,
//			assuming that the path and name of
//			the FX file is the same, /w ext: .mfx
//			NOTE: the vector passed in is only used
//			by Model FX Editor.
// Output:	FX intialized
//			txtPaths is filled with the FULL PATH of
//			textures stage per group (it's a little crazy...)
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE MDLFXLoad(hMDL mdl, const char *filename, 
						 vector< vector<string> > *txtPaths = 0);

//
//object functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJCreate(unsigned int newID, hMDL model, float x, float y, float z, 
					  eOBJTYPE objType, const char *animationfile);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJAddRef(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJSearch(unsigned int ID);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJDestroy(hOBJ *obj);

/////////////////////////////////////
// Name:	OBJTranslate
// Purpose:	translate object, bAdd is
//			used for moving
// Output:	object translated
// Return:	none
/////////////////////////////////////
PUBLIC void OBJTranslate(hOBJ obj, float trans[eMaxPt], bool bAdd);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJRotX(hOBJ obj, float radian);
PUBLIC void OBJRotY(hOBJ obj, float radian);
PUBLIC void OBJRotZ(hOBJ obj, float radian);

PUBLIC void OBJRot(hOBJ obj, float rot[eMaxPt], bool bAdd);

/////////////////////////////////////
// Name:	OBJScale
// Purpose:	scale the object
// Output:	obj scale
// Return:	none
/////////////////////////////////////
PUBLIC void OBJScale(hOBJ obj, float s[eMaxPt], bool bAdd);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJDisplay(hOBJ obj, bool bApplyLight=false);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJUpdateFrame(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int OBJGetCurState(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJSetState(hOBJ obj, int state);

/////////////////////////////////////
// Name:	OBJIsStateEnd
// Purpose:	checks to see if the state
//			of an object reached the end
//			frame of current state
// Output:	none
// Return:	true if curframe reached end
/////////////////////////////////////
PUBLIC bool OBJIsStateEnd(hOBJ obj);

/////////////////////////////////////
// Name:	OBJPause
// Purpose:	pause / resume  object
//			(true)  (false)
// Output:	Obj flag is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJPause(hOBJ obj, bool bPause);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJPauseOnStateEnd(hOBJ obj, bool bPause);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJAddChild(hOBJ obj, hOBJ child, int parentJointInd=-1, bool syncState=true);

/////////////////////////////////////
// Name:	OBJRemoveFromParent
// Purpose:	remove object from it's
//			parent, if it has one
// Output:	parent free object
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE OBJRemoveFromParent(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJRemoveChild(hOBJ obj, hOBJ child);

/////////////////////////////////////
// Name:	OBJGetModel
// Purpose:	grabs model of obj.
// Output:	none
// Return:	the model of obj.
/////////////////////////////////////
PUBLIC hMDL OBJGetModel(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetLoc
// Purpose:	grabs the location of 
//			the obj.  This is only
//			the translation(local/world)
// Output:	loc is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetLoc(hOBJ obj, float loc[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetWorldLoc
// Purpose:	grabs the actual world 
//			location
// Output:	loc
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetWorldLoc(hOBJ obj, float loc[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetScale
// Purpose:	grabs the scale value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetScale(hOBJ obj, float scl[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetRotate
// Purpose:	grabs the rotate value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetRotate(hOBJ obj, float rot[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumFace(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumVtx(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetNumModel	
// Purpose:	This will give you the num
//			of models associated with
//			the obj.  This will go through
//			the children as well.
// Output:	none
// Return:	num model
/////////////////////////////////////
PUBLIC int OBJGetNumModel(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetBounds
// Purpose:	fills up the bounds array
//			with bound info of obj and
//			all it's children.
//			NOTE:  Assumes that bounds
//			is allocated to the number of
//			models associated w/ object
// Output:	bounds is filled
// Return:	count of bounds filled
/////////////////////////////////////
PUBLIC int OBJGetBounds(hOBJ obj, gfxBound *bounds, bool bGet1stOnly=false);

/////////////////////////////////////
// Name:	OBJIntersectLine
// Purpose:	checks to see if the given
//			object is intersecting a line
//			give the bounding box (min/max)
//			if you want box to box collision
// Output:	time 't' between two points
// Return:	true if collided
/////////////////////////////////////
PUBLIC bool OBJIntersectLine(hOBJ obj, float mins[eMaxPt], float maxs[eMaxPt],
								  float pt1[eMaxPt], float pt2[eMaxPt], 
								  float *tOut, float nOut[eMaxPt]);

/////////////////////////////////////
// Name:	OBJFixOrientation
// Purpose:	fixes obj's orientation
// Output:	object rotated to -90
// Return:	none
/////////////////////////////////////
PUBLIC void OBJFixOrientation(hOBJ obj);

/////////////////////////////////////
// Name:	OBJIsDeactivate
// Purpose:	check to see if given
//			obj is deactivate
// Output:	none
// Return:	true if deactivate
/////////////////////////////////////
PUBLIC bool OBJIsDeactivate(hOBJ obj);

/////////////////////////////////////
// Name:	OBJDeactivate
// Purpose:	activates/deactivates obj
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void OBJActivate(hOBJ obj, bool bActivate);

/////////////////////////////////////
// Name:	OBJDuplicate
// Purpose:	duplicates a given object
// Output:	stuff happens
// Return:	new clone object
/////////////////////////////////////
PUBLIC hOBJ OBJDuplicate(hOBJ obj, unsigned int newID, float x, float y, float z);

/////////////////////////////////////
// Name:	OBJSetOrientation
// Purpose:	set the orient of obj
// Output:	orient is done
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetOrientation(hOBJ obj, float vec[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetOrientation
// Purpose:	get the orient of obj
// Output:	the vec is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetOrientation(hOBJ obj, float vec[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int OBJGetID(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC gfxID OBJGetKey(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJQuery(const gfxID *key);

/////////////////////////////////////
// Name:	OBJSetTexture
// Purpose:	change obj's
//			texture within material
//			index
// Output:	obj's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE OBJSetTexture(hOBJ obj, int materialInd, hTXT texture);

/////////////////////////////////////
// Name:	OBJGetNumMaterial
// Purpose:	get the number of materials
// Output:	none
// Return:	number of materials
/////////////////////////////////////
PUBLIC int OBJGetNumMaterial(hOBJ obj);

/////////////////////////////////////
// Name:	OBJSetMaterial
// Purpose:	change obj's
//			material within material
//			index
// Output:	obj's material change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE OBJSetMaterial(hOBJ obj, int materialInd, const GFXMATERIAL * material);
PUBLIC void OBJGetMaterial(hOBJ obj, int materialInd, GFXMATERIAL * materialOut);

/////////////////////////////////////
// Name:	OBJSetAlpha
// Purpose:	set the object alpha channel
//			value ranges from 0~1
// Output:	materials are set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetAlpha(hOBJ obj, float alphaVal);

/////////////////////////////////////
// Name:	OBJJointGetIndex
// Purpose:	get index of given joint
//			name
// Output:	none
// Return:	joint index, -1 if not found
/////////////////////////////////////
PUBLIC int OBJJointGetIndex(hOBJ obj, const char *jointName);

/////////////////////////////////////
// Name:	JointGetTranslate
// Purpose:	get the joint location
//			(model space)
// Output:	ptOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJJointGetTranslate(hOBJ obj, int ind, float ptOut[eMaxPt]);

/////////////////////////////////////
// Name:	OBJJointGetWorldLoc
// Purpose:	get the world location
//			of a joint
// Output:	ptOut
// Return:	none
/////////////////////////////////////
PUBLIC void OBJJointGetWorldLoc(hOBJ obj, int ind, float ptOut[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJJointTranslate(hOBJ obj, int ind, float trans[eMaxPt], bool bAdd=false);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJJointRotate(hOBJ obj, int ind, float rot[eMaxPt], bool bAdd=false);

/////////////////////////////////////
// Name:	OBJSetDrawBound
// Purpose:	set the draw bound of the
//			given object
// Output:	object draw bound set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetDrawBound(hOBJ obj, float mins[eMaxPt], float maxs[eMaxPt]);

/////////////////////////////////////
// Name:	OBJSetToLast
// Purpose:	set the given object to
//			be the last item in the
//			object render list
// Output:	object list updated
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetToLast(hOBJ obj);

/////////////////////////////////////
// Name:	OBJSetAnimScale
// Purpose:	set the animation time
//			scale. (usu.: 1)
// Output:	sets the speed of animatin
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetAnimScale(hOBJ obj, double s);

//
//font functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hFNT FontCreate(const char *typeName, unsigned int size, unsigned int fmtFlag);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontResize(hFNT fnt, unsigned int size);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf2D(hFNT fnt, float x, float y, unsigned int color, const char *text, ...);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf3D(hFNT fnt, D3DXMATRIX *wrldMtx, GFXMATERIAL *mtrl, 
							DWORD flags, const char *text, ...);

/////////////////////////////////////
// Name:	FontTextBox
// Purpose:	display font in a box
// Output:	font blt in a box
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE FontTextBox(hFNT fnt, float sX, float sY, 
						   float eX, float eY, unsigned int color, 
						   const char *text);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontDestroy(hFNT *fnt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetStrSize(hFNT fnt, char *str, SIZE *pSize);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetCharSize(hFNT fnt, char c, SIZE *pSize);

//
//sprite functions
//

/////////////////////////////////////
// Name:	SpriteCreate
// Purpose:	creates a sprite with txt.
//			NOTE: states has to be allocated with
//				  size numState.
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
PUBLIC hSPRITE SpriteCreate(hTXT txt, int numFrame, SIZE frameSize, 
							double delay, int numImgCol, LOCATION *imgOffset,
							int numState, const gfxSprFrame *states);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hSPRITE SpriteLoad(hTXT txt, int numImageCol, LOCATION *imgOffset,
						  const char *filename);

/////////////////////////////////////
// Name:	SpriteDestroy
// Purpose:	destroys sprite
// Output:	none
// Return:	none
/////////////////////////////////////
PUBLIC void SpriteDestroy(hSPRITE *spr);

/////////////////////////////////////
// Name:	SpriteIsStateEnd
// Purpose:	checks to see if the state
//			of a sprite reached the end
//			frame of current state
// Output:	none
// Return:	true if curframe reached end
/////////////////////////////////////
PUBLIC bool SpriteIsStateEnd(hSPRITE spr);

/////////////////////////////////////
// Name:	SpritePause
// Purpose:	pause/unpause sprite
// Output:	spr state set
// Return:	none
/////////////////////////////////////
PUBLIC void SpritePause(hSPRITE spr, bool bPause);

/////////////////////////////////////
// Name:	SpritePauseOnStateEnd
// Purpose:	pause animation on end
// Output:	sprite state set
// Return:	
/////////////////////////////////////
PUBLIC void SpritePauseOnStateEnd(hSPRITE spr, bool bPause);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void SpriteUpdateFrame(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void SpriteUpdateFrameAllState(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int SpriteGetMaxState(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int SpriteGetCurState(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE SpriteSetState(hSPRITE spr, int state);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC SIZE SpriteGetSize(hSPRITE spr);

/////////////////////////////////////
// Name:	SpriteBlt
// Purpose:	displays sprite
//			NOTE: GFXBltModeEnable()
//				  must be called first
// Output:	sprite is displayed
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
PUBLIC RETCODE SpriteBlt(hSPRITE spr, const float loc[eMaxPt], const DWORD *color, float rotate);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE SpriteStretchBlt(hSPRITE spr, const float loc[eMaxPt],
								float w, float h, const DWORD *color, float rotate);

/////////////////////////////////////
// Name:	SpriteBlt3D
// Purpose:	displays sprite
//			NOTE: GFXBegin()
//				  must be called first
//			This will also set cull mode
//			to none
// Output:	sprite is displayed in 3d
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
PUBLIC RETCODE SpriteBlt3D(hSPRITE spr, const D3DXMATRIX *wrldMtx, 
						   const GFXMATERIAL *material,
						   const DWORD *color);

/////////////////////////////////////
// Name:	SpriteDuplicate
// Purpose:	duplicate a sprite with
//			given sprite
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
PUBLIC hSPRITE SpriteDuplicate(hSPRITE spr);

//
//map functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTMAP TMapCreateRand(float celSizeX, float celSizeZ, float height,
							int xDir, int zDir, float r, unsigned int numIter,
							hTXT texture, GFXMATERIAL *mat);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TMapDisplay(hTMAP map, int cBeginCol, int cBeginRow,
						   int cEndCol, int cEndRow);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TMapDestroy(hTMAP *map);

/////////////////////////////////////
// Name:	TMapGetSize
// Purpose:	grabs the cel size of the
//			map.  eX/eZ is what you want
// Output:	size is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetSize(hTMAP map, float size[eMaxPt]);

/////////////////////////////////////
// Name:	TMapGetCelVtx
// Purpose:	gets the quad vtx of the
//			specified cel of map
// Output:	vtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetCelVtx(hTMAP map, int ind, float vtx[NUMPTQUAD][eMaxPt]);

/////////////////////////////////////
// Name:	TMapClearCelTxt
// Purpose:	clear cel texture
// Output:	cel texture removed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapClearCelTxt(hTMAP map, int ind);

/////////////////////////////////////
// Name:	TMapSetCelTxt
// Purpose:	set cel's texture
// Output:	cel's texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TMapSetCelTxt(hTMAP map, int ind, hTXT txt);

/////////////////////////////////////
// Name:	TMapSetRenderType
// Purpose:	sets map to render in
//			lines for now...
// Output:	map status changed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapSetRenderType(hTMAP map, unsigned int passInZeroForNow);

/////////////////////////////////////
// Name:	TMapResetRenderType
// Purpose:	sets map to normal render
// Output:	map status changed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapResetRenderType(hTMAP map);

/////////////////////////////////////
// Name:	TMapGetInd
// Purpose:	grabs the index from given
//			loc.
// Output:	none
// Return:	index within map (ind<0 for outofbound)
/////////////////////////////////////
PUBLIC int TMapGetInd(hTMAP map, float Point [eMaxPt], float dirX, float dirZ);

/////////////////////////////////////
// Name:	TMapGetPlaneEq
// Purpose:	grabs the two planes of
//			a cel
// Output:	pl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetPlaneEq(hTMAP map, int ind, float pl[2][eMaxPlaneEq]);

/////////////////////////////////////
// Name:	TMapGetIndSize
// Purpose:	grabs the index size of
//			map (row,col)
// Output:	none
// Return:	index size
/////////////////////////////////////
PUBLIC SIZE TMapGetIndSize(hTMAP map);

/////////////////////////////////////
// Name:	TMapGetTexture
// Purpose:	grabs the terrain texture
// Output:	none
// Return:	terrain texture
/////////////////////////////////////
PUBLIC hTXT TMapGetTexture(hTMAP map);

/////////////////////////////////////
// Name:	TMapSetTxtSprRand
// Purpose:	makes your map look cool
// Output:	map texture is changed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE TMapSetTxtSprRand(hTMAP map, hSPRITE spr);

//
// Light functions
//

/////////////////////////////////////
// Name:	GFXSetMaxLight
// Purpose:	sets the max light, must be
//			at max 8
// Output:	max light set
// Return:	
/////////////////////////////////////
PUBLIC void GFXSetMaxLight(int numLight);

/////////////////////////////////////
// Name:	GFXEnableLight
// Purpose:	enable/disable light
//			also set ambient color
// Output:	light is enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnableLight(bool do_you_want_light);

/////////////////////////////////////
// Name:	GFXIsLightEnabled
// Purpose:	is light enabled?
// Output:	none
// Return:	true if so...
/////////////////////////////////////
PUBLIC bool GFXIsLightEnabled();

/////////////////////////////////////
// Name:	Light(Set/Get)Ambient
// Purpose:	set/get the ambient
// Output:	ambient if 'Get'
// Return:	none
/////////////////////////////////////
PUBLIC void LightSetAmbient(DWORD ambient);
PUBLIC void LightGetAmbient(DWORD * ambient);

/////////////////////////////////////
// Name:	LightIsEnable
// Purpose:	checks to see if given
//			light is enabled
// Output:	none
// Return:	TRUE if enabled
/////////////////////////////////////
PUBLIC BOOL LightIsEnable(const gfxID * ID);

/////////////////////////////////////
// Name:	LightEnable
// Purpose:	enables/disables light
// Output:	enable/disable light
// Return:	none
/////////////////////////////////////
PUBLIC void LightEnable(const gfxID * ID, DWORD lightInd, BOOL enable);

/////////////////////////////////////
// Name:	LightSet
// Purpose:	set light
// Output:	light is set
// Return:	none
/////////////////////////////////////
PUBLIC void LightSet(const gfxID * ID, GFXLIGHT *lightStruct);
PUBLIC void LightSet(DWORD lightInd, GFXLIGHT *lightStruct, BOOL enable);

/////////////////////////////////////
// Name:	LightCreate
// Purpose:	adds new light to GFX
// Output:	increment number of light
// Return:	ID of new light
/////////////////////////////////////
PUBLIC gfxID LightCreate(GFXLIGHT *lightStruct);

/////////////////////////////////////
// Name:	LightReset
// Purpose:	disables all light and
//			resets number of lights
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void LightReset();

/////////////////////////////////////
// Name:	LightDestroy
// Purpose:	removes light
// Output:	decrease number of light
//			disables given light ind
// Return:	none
/////////////////////////////////////
PUBLIC void LightDestroy(const gfxID * ID);

/////////////////////////////////////
// Name:	LightDestroyAll
// Purpose:	destroys all light, making all
//			lights invalid
// Output:	all lights dead
// Return:	none
/////////////////////////////////////
PUBLIC void LightDestroyAll();

/////////////////////////////////////
// Name:	LightGetData
// Purpose:	grabs given light's data
// Output:	lightStruct is filled
// Return:	none
/////////////////////////////////////
PUBLIC void LightGetData(const gfxID * ID, GFXLIGHT *lightStruct);

/////////////////////////////////////
// Name:	LightEnableAllNearest
// Purpose:	enable lights that are near
//			location.
// Output:	lights enabled
// Return:	none
/////////////////////////////////////
PUBLIC void LightEnableAllNearest(const float loc[eMaxPt]);



//
// Fog stuff
//

/////////////////////////////////////
// Name:	GFXEnableFog
// Purpose:	enable/disable the fog
// Output:	fog enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnableFog(bool bYes);

/////////////////////////////////////
// Name:	GFXIsFogEnable
// Purpose:	check to see if fog is
//			enabled
// Output:	none
// Return:	true if enabled
/////////////////////////////////////
PUBLIC bool GFXIsFogEnable();

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
PUBLIC void FogSetMode(DWORD mode);

/////////////////////////////////////
// Name:	FogSetColor
// Purpose:	set color of fog
// Output:	color of fog is set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetColor(DWORD clr);

/////////////////////////////////////
// Name:	FogSetStart
// Purpose:	set the start of fog,
//			used in linear mode
// Output:	fog start set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetStart(float val);

/////////////////////////////////////
// Name:	FogSetEnd
// Purpose:	set the end of fog,
//			used in linear mode
// Output:	fog end set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetEnd(float val);

/////////////////////////////////////
// Name:	FogSetDensity
// Purpose:	set the density of fog,
//			used in exp/exp2 mode
// Output:	fog density set
// Return:	none
/////////////////////////////////////
PUBLIC void FogSetDensity(float val);

//
// Math stuff
//

/////////////////////////////////////
// Name:	GFXMathCreateRayFromLoc
// Purpose:	create a ray from given 
//			screen loc
// Output:	ray pos&dir is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXMathCreateRayFromLoc(const LOCATION *loc,
										D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir);

/////////////////////////////////////
// Name:	GFXIntersectBox
// Purpose:	not really related to
//			anything Graphics but nice
//			to have.  Intersect given
//			ray to axis aligned bound box.
// Output:	tOut and nOut filled
// Return:	true if tOut < 1
/////////////////////////////////////
PUBLIC bool GFXIntersectBox(float p[eMaxPt], float dir[eMaxPt], 
				  float boxMin[eMaxPt], float boxMax[eMaxPt],
				  float *tOut, float nOut[eMaxPt] = 0);

//
// Primitives
//

/////////////////////////////////////
// Name:	GFXDrawBoxXZ
// Purpose:	draws a box from xz plane
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawBoxXZ(float xMin, float zMin, float xMax, float zMax, 
						 float h, unsigned int color);

/////////////////////////////////////
// Name:	GFXDrawPixel
// Purpose:	draws a 2d pixel
// Output:	yay
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawPixel(float x, float y, unsigned int clr);

/////////////////////////////////////
// Name:	GFXDrawBox2D
// Purpose:	draws a 2d filled box
// Output:	box
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawBox2D(const RECT *pRect, DWORD clr);

//
// Misc
//

PUBLIC void GFXHideCursor (void);
PUBLIC void GFXShowCursor (void);

//
// Sky Box
//

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
						  hTXT textureLeft, hTXT textureRight);

/////////////////////////////////////
// Name:	SkyBoxRender
// Purpose:	renders the skybox
// Output:	sky box rendered
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE SkyBoxRender();

/////////////////////////////////////
// Name:	SkyBoxDestroy
// Purpose:	destroys the skybox
// Output:	skybox destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void SkyBoxDestroy();

//
// Screen stuff
//

/////////////////////////////////////
// Name:	GFX_ScreenCapture
// Purpose:	take a snapshot of what
//			currently is on screen
// Output:	screen stuff filled
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFX_ScreenCapture();

/////////////////////////////////////
// Name:	GFX_ScreenDisplay
// Purpose:	display the snapshot of
//			the screen
//			NOTE: call GFXBltModeEnable
//				  first!!!
// Output:	stuff displayed
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE GFX_ScreenDisplay(const DWORD *color, float rotate);

//
// View Frustrum
//

/////////////////////////////////////
// Name:	FrustrumCheckPoint
// Purpose:	check point in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckPoint(float x, float y, float z);

/////////////////////////////////////
// Name:	FrustrumCheckSphere
// Purpose:	check sphere in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckSphere(float x, float y, float z, float r);

/////////////////////////////////////
// Name:	FrustrumCheckCube
// Purpose:	check cube in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckCube(float x, float y, float z, float s);

/////////////////////////////////////
// Name:	FrustrumCheckBox
// Purpose:	check box in frustrum
// Output:	none
// Return:	true if so
/////////////////////////////////////
PUBLIC bool FrustrumCheckBox(float x1, float y1, float z1,
							 float x2, float y2, float z2);

/////////////////////////////////////
// Name:	FrustrumCheckBoxEX
// Purpose:	check box in frustrum
// Output:	none
// Return:	BBOX_IN
//			BBOX_OUT
//			BBOX_INTERSECT
/////////////////////////////////////
PUBLIC int FrustrumCheckBoxEX(float x1, float y1, float z1,
							 float x2, float y2, float z2);

//
// QBSP Quake 3!
//

/////////////////////////////////////
// Name:	QBSPLoad
// Purpose:	loads a .bsp file and
//			create a qbsp map
// Output:	none
// Return:	new qbsp, 0 if failure
/////////////////////////////////////
PUBLIC hQBSP QBSPLoad(const char *filename, const char *mainPath,
					  ENTITYPARSEFUNC parseFunc, void *userDat);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void QBSPDestroy(hQBSP *qbsp);

/////////////////////////////////////
// Name:	QBSPUseVertexLighting
// Purpose:	enable/disable vertex lighting
//			QBSP will use lightmap if
//			vertex lighting is disabled
// Output:	vertex lighting set
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPUseVertexLighting(hQBSP qbsp, bool bEnable);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE QBSPRender(hQBSP qbsp, hCAMERA cam);

/////////////////////////////////////
// Name:	QBSPFindLeaf
// Purpose:	find the leaf index where
//			the given pt resides.
// Output:	none
// Return:	leaf index >= 0 if found
/////////////////////////////////////
PUBLIC int QBSPFindLeaf(hQBSP qbsp, const float pt[eMaxPt]);

/////////////////////////////////////
// Name:	QBSPCollBrush
// Purpose:	collide with brush
//			NOTE: pTrace->t must have
//			a value (usu. 1)!
// Output:	pTrace, if t < 1, then
//			you better do something
//			about it!
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPCollBrush(hQBSP qbsp, int brushIndex, bool bIsPt, 
							  float mins[eMaxPt], float maxs[eMaxPt], 
							  float p[eMaxPt], float dir[eMaxPt], 
							  gfxTrace *pTrace);

/////////////////////////////////////
// Name:	QBSPCollision
// Purpose:	find collision with the given
//			map.  Will collide if 't' is
//			within [0,1)
//			NOTE: testT is the starting
//				  't' value (starting minimum)
//				  usu. set as 't = 1'
//				  startLeafIndex is index 
//				  in leaf (-1 to rescan).
// Output:	pTrace filled
// Return:	true if collide
/////////////////////////////////////
PUBLIC void QBSPCollision(hQBSP qbsp, float mins[eMaxPt], float maxs[eMaxPt],
						  float pt1[eMaxPt], float pt2[eMaxPt], gfxTrace *pTrace,
						  float testT = 1, int startLeafIndex = -1);

/////////////////////////////////////
// Name:	QBSPGetTexture
// Purpose:	get the texture
// Output:	none
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT __fastcall QBSPGetTexture(hQBSP qbsp, int txtInd);

/////////////////////////////////////
// Name:	QBSPGetTextureFlags
// Purpose:	grab the flags of texture
//			(based on QBSP texture flags)
// Output:	none
// Return:	flags
/////////////////////////////////////
PUBLIC int __fastcall QBSPGetTextureFlags(hQBSP qbsp, int txtInd);

/////////////////////////////////////
// Name:	QBSPGetTextureContents
// Purpose:	grab the contents of texture
//			(based on QBSP texture contents)
// Output:	none
// Return:	contents
/////////////////////////////////////
PUBLIC int __fastcall QBSPGetTextureContents(hQBSP qbsp, int txtInd);

/////////////////////////////////////
// Name:	QBSPGetNumLeaves
// Purpose:	get number of leaves
// Output:	none
// Return:	the number of leaves.
/////////////////////////////////////
PUBLIC int QBSPGetNumLeaves(hQBSP qbsp);

/////////////////////////////////////
// Name:	QBSPRayModelGetBBoxMin
// Purpose:	get the bbox
// Output:	pOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPRayModelGetBBoxMin(hQBSP qbsp, int modelInd, float pOut[eMaxPt]);

/////////////////////////////////////
// Name:	QBSPRayModelGetBBoxMax
// Purpose:	get the bbox
// Output:	pOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPRayModelGetBBoxMax(hQBSP qbsp, int modelInd, float pOut[eMaxPt]);

/////////////////////////////////////
// Name:	QBSPRayModelGetRadius
// Purpose:	get radius of the model
// Output:	none
// Return:	the radius
/////////////////////////////////////
PUBLIC float QBSPRayModelGetRadius(hQBSP qbsp, int modelInd);

/////////////////////////////////////
// Name:	QBSPModelCollision
// Purpose:	find collision with the given
//			model inside the map.
//			NOTE: testT is the starting
//				  't' value (starting minimum)
//				  usu. set as 't = 1'
// Output:	pTrace filled
// Return:	true if collided
/////////////////////////////////////
PUBLIC bool QBSPModelCollision(hQBSP qbsp, int modelInd, 
								  float mins[eMaxPt], float maxs[eMaxPt],
								  float pt1[eMaxPt], float pt2[eMaxPt], 
								  gfxTrace *pTrace, float testT = 1);

/////////////////////////////////////
// Name:	QBSPModelTranslate
// Purpose:	translate the model
// Output:	model is translated
//			as well as it's planes
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelTranslate(hQBSP qbsp, int modelInd, float trans[eMaxPt], bool bAdd);

/////////////////////////////////////
// Name:	QBSPModelRotate
// Purpose:	rotate the model
// Output:	model is rotated
//			as well as it's planes
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelRotate(hQBSP qbsp, int modelInd, float rot[eMaxPt], bool bAdd);

/////////////////////////////////////
// Name:	QBSPModelEnable
// Purpose:	enable/disable model
//			If disabled, model will
//			not perform collision
//			and will not be displayed
// Output:	model status set
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelEnable(hQBSP qbsp, int modelInd, bool bEnable);

/////////////////////////////////////
// Name:	QBSPModelGetTranslate
// Purpose:	get the translate of 
//			the model
// Output:	ptOut
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPModelGetTranslate(hQBSP qbsp, int modelInd, float ptOut[eMaxPt]);

#endif