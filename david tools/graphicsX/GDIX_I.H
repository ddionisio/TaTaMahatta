#ifndef _gdix_i_h
#define _gdix_i_h

#include "gdix.h"
#include "gdix_types.h"
//#include "GFX_Bounds.h"

//#include "..\Memory\memory.h"
#define GFX_MALLOC(s) calloc(1, (s))//malloc((s))
#define GFX_FREE(data) free((data))

#include "..\Parser\parser.h"
#include "..\Timer\timer.h"

#include "d3d9.h"
#include "D3dx9tex.h"
#include "d3dx9mesh.h"
#include "d3dx9core.h"

//#include <mmsystem.h>	//for the sake of that damn dxutil
//#include "dxutil.h"		//might as well use it...

#define INTERPOLATEANIM

//internal globals
extern PGFXMAIN		 g_pD3D;	  //the mac daddy of d3d
extern PGFXDEVICE	 g_p3DDevice; //the main interface object
extern ID3DXMatrixStack *g_pWrldStack;//the world matrix stack
//extern gfxDisplayMode	 g_DISPLAY;	  //our current display mode
extern RECT				 g_CLIENTRECT;//window rectangle (only useful in window mode)
extern FLAGS			 g_FLAGS;	  //status of graphicsX eg: GFX_FULLSCREEN

extern LISTPTR			 g_TXTLIST;				//list of textures
extern LISTPTR			 g_MDLLIST;				//list of models
extern LISTPTR			 g_OBJLIST;				//list of objects
extern LISTPTR			 g_OBJDEACTIVATELIST;	//list of deactivated objects

extern char				 g_MDLLISTFILE[DMAXCHARBUFF]; //the model list file
extern char				 g_TXTLISTFILE[DMAXCHARBUFF]; //the texture list file

extern LPD3DXSPRITE			   g_pTxtSprite; //global sprite interface

extern float g_sinTable[SINCOUNT];

extern GFXCAPS g_d3dcaps;

extern D3DXMATRIX		 g_projMtx;

//sigh...
extern D3DXVECTOR3		g_eyePt;		//camera eye pt., set by CameraSetScene

//internal data structures
typedef struct _gfxWrldMtx {
	D3DXMATRIX   mtxTrans;			 //Translation matrix
	D3DXMATRIX   mtxRot;			 //Rotation matrix
	D3DXMATRIX	 mtxScale;			 //Scale matrix
	D3DXMATRIX   mtxWrld;			 //World matrix (scale * rot * trans)
} gfxWrldMtx;

typedef struct _gfxCamera {
	D3DXMATRIX mtxCamera;	//camera matrix to apply
	D3DXVECTOR3 vEye;		//the eye position
	D3DXVECTOR3 vTarget;	//the eye target
	D3DXVECTOR3 vDir;		//the eye direction (forward vector)
	D3DXVECTOR3 vUp;		//the 'up' vector (always the same)
	angle aYaw,aPitch,aRoll;//angle in degree
} gfxCamera;

typedef struct _gfxTexture {
	unsigned int ID;				//Identification
	char	 name[NAMEMAX];				//name of texture

	PGFXTEXTURE texture;		//It's the d3d texture!
	int width;						//width of texture
	int height;						//height of texture

	int ref;						 //number of things refering to this

	bool	bFile;			//true if this texture is loaded from file

	D3DCOLOR srcClrKey;		//if this is used
	bool	 bClrKey;		//if uses color key
} gfxTexture;

//////////////////////////////////////
//	Mesh FX for each group
typedef struct _FXRenderState {
	DWORD		state;		//the state to change
	DWORD		value;		//the new value for the state
	DWORD		oldValue;	//the value before setting the render state
} FXRenderState;

typedef vector<FXRenderState> FXRenderStateArray;
typedef vector< vector<FXRenderState> > FXRenderStateDArray;

typedef struct _gfxFXTxtAnimDat {
	float	s, t;			//(s,t)
	
	float	spd_s, spd_t;	//speed (s,t)
	
	float	acc_s, acc_t;	//acceleration (s,t)
	
	float	min_s, min_t;	//min (s,t)

	float	max_s, max_t;	//max (s,t)
} gfxFXTxtAnimDat;

typedef struct _gfxFXTxtAnim {
	bool				bEnable;

	gfxFXTxtAnimDat		scale;
	gfxFXTxtAnimDat		rotate;
	gfxFXTxtAnimDat		trans;

	gfxWrldMtx			txt_mtx;
} gfxFXTxtAnim;

typedef struct _gfxMeshFX {
	FXRenderStateArray	*renderStates;		//render states
	FXRenderStateDArray	*textureStates;		//texture states

	gfxFXTxtAnim		txtAnim[NUMVTXTEXTURE];
	hTXT				txt[NUMVTXTEXTURE];	//texture to use per stage
} gfxMeshFX;
//////////////////////////////////////

typedef struct _gfxMeshGrp {
	BYTE	flag;					//some flag
	char	name[NAMEMAX2];			//name

	GFXINDBUFF *indBuff;	//index buffer
	DWORD	numInd;					//number of indices (faceCount*NUMPTFACE)
	DWORD	faceCount;				//number of primitives

	int		materialInd;			//index to material & texture (-1 for no material)

	gfxMeshFX	*mdlFX;				//effect applied to this group (if not NULL)
} gfxMeshGrp;

typedef struct _gfxMesh {
	GFXVTXBUFF			   *vtxBuff;		//The vertex buffer
	gfxVtx				   *vtxBuffSys;		//System Memory Vertex Buffer

	char				   *boneVtxIDs;		//Bone IDs (numVtx), -1 for no bone

	DWORD				    numVtx;			//number of veritces

	gfxMeshGrp			   *grp;			//Group of indices (numGrp)
	DWORD				    numGrp;			//number of groups
	
	DWORD			        maxIndCount;	//total number of indices
	DWORD				    maxFaceCount;	//total number of faces

	DWORD					vtxBuffFlag;	//flags used to create the vtx buffer
} gfxMesh;

typedef struct _jointFrame {
	float			trans[eMaxPt];		//translation
	D3DXQUATERNION	quat;				//quaternion, used by rotation
} jointFrame;

typedef struct _gfxModelJoint {			//need a joint?
	char		name[NAMEMAX2];			//name of joint

	int			parent;					//parent of joint (-1 for none)
	
	float		localRotate[eMaxPt];	//local rotation
	float		localTrans[eMaxPt];		//local translation

	D3DXMATRIX	absolute;				//mtx to get from (0,0,0) to the joint following the hierachy
	D3DXMATRIX	relative;				//mtx to get from the parent joint to this joint.

	jointFrame		*jointFrames;	//joint transformations for each frame (numFrames determined by model)
} gfxModelJoint;

typedef struct _gfxModelFrame {
	char		 name[FRAMEMAXCHAR]; //name of frame (usually used by MD2)
	gfxMesh		 frameMesh;		 //key frame mesh
} gfxModelFrame;

typedef struct _gfxModel {
	unsigned int ID;				 //Identification
	char	 name[NAMEMAX];				 //name of model
	
	unsigned int numMaterial;		 //number of materials for this model

	unsigned int baseInd;			 //zero
	
	gfxModelFrame		*frames;	 //if model is animated, contains key frame meshes (numFrame)
	unsigned int  numFrames;		 //number of animation frame

	hTXT		 *textures;			 //array of textures (numMaterial)
	GFXMATERIAL *materials;		 //array of materials (numMaterial)

	gfxBound	 bound;				 //boundary of model

	int ref;						 //number of things refering to this

	gfxMesh		 mesh;				//mesh to be displayed

	gfxModelJoint	*joints;	//joints for the model
	unsigned int    numJoints;  //number of joints

	FLAGS status;					 //status of model
} gfxModel;

typedef struct _gfxObjMdlState {
	int startFrame;	//starting frame
	int endFrame;	//ending frame

	double delay;	//millisecond it takes to get from one frame to the next
	float maxDelay;	//millisecond it takes to get from start to end frame
} gfxObjMdlState;

typedef struct _gfxObjJoint {
	D3DXMATRIX		final;				//mtx to apply on each vtx.
	D3DXMATRIX		userMtx;			//user transformation

	float        userTrans[eMaxPt];	 //user translation
	float		 userRot[eMaxPt];	 //user rotation (RADIANS: along x,y,z)
} gfxObjJoint;

typedef struct _gfxObject {
	unsigned int ID;				 //Identification
	gfxID		 key;				 //Unique identification (safer than pointers!)

	hMDL	     theMdl;			 //Model this object use
	gfxMesh		 *curMesh;			 //current mesh to be displayed (it's just a pointer)

	gfxObjJoint	 *joints;			 //object joints corresponding to the model (use for animation)

	hTXT		 *textures;			 //array of textures corresponding to the model(numMaterial)
	GFXMATERIAL *materials;		 //array of materials corresponding to the model(numMaterial)
	
	win32Time	 ticker;			 //delay n' stuff 'bout time

	gfxObjMdlState *states;			 //states (numState)
	unsigned int numStates;			 //number of states
	unsigned int curState;			 //current state

	unsigned int curFrame;			 //current frame
	unsigned int prevFrame;			 //previous frame, used by interpolation...

	float        wrldTrans[eMaxPt];	 //world translation
	float		 wrldRot[eMaxPt];	 //world rotation (RADIANS: along x,y,z)
	float		 wrldScl[eMaxPt];	 //world scale    (DEFAULT: x=1,y=1,z=1)

	gfxWrldMtx   wrldMtx;			 //The object world matrix (world loc/rot)

	int ref;						 //number of things refering to this

	gfxID		 parentKey;			 //if this object is a child, then parentKey is it's parent ID
	int			 parentJointInd;	 //the joint root of this child if not -1

	LISTPTR		 *objNode;			 //list of children attached to this object
	LISTPTR		 *listRef;			 //pointer to list this object is in

	D3DXVECTOR3  orient;			 //orientation of object

	float		 drawBoundMin[eMaxPt];	//this is the draw bound min for frustrum culling
	float		 drawBoundMax[eMaxPt];  //this is the draw bound max for frustrum culling
	float		 drawBoundRadius;		//this is the draw radius for distance culling

	double		 animeScale;			//scale for animation (default = 1)

	FLAGS status;					 //object flags
} gfxObject;

typedef struct _gfxFont {
	CD3DFont*     pD3DFont;				//The D3D Font
	TCHAR         strFont[MAXCHARBUFF];	//The font type name
	unsigned int  fontSize;				//Size of font
	unsigned int  fntFormat;			//format of font: BOLD, ITALIC
} gfxFont;

typedef struct _gfxSprite {
	hTXT		  theTexture;			//handle to texture this sprite use
	win32Time	  ticker;				//delay n' stuff 'bout time
	int numFrames;						//Number of state frames
	int curState;						//Index of current state
	int numStates;						//Number of sprite states
	SIZE frameSize;						//The size of each frame
	int numImgCol;						//Mainly used for duplication
	LOCATION imgOffset;					//Mainly used for duplication
	RECT *frameRects;					//Array of rects for each frames
	gfxSprFrame *states;				//Array of sprite states
	FLAGS status;						//sprite flags
} gfxSprite;

typedef struct _gfxCel {
	GFXVTXBUFF	 *vtx;					//Vertex buffer
	int numVtx;							//usually 4
	hTXT subTxt;						//overlay texture
	float plane[2][eMaxPlaneEq];		//two plane equations
} gfxCel;

//Terrain Map
typedef struct _gfxTMap {
	SIZE indSize;						//Size of array (col*row)
	float celSize[eMaxPt];				//Size of each cel
	float wrldTrans[eMaxPt];			//world position
	gfxWrldMtx   wrldMtx;				//The world matrix (ignore rotation!)
	gfxCel *cels;						//array of cel (col*row)
	hTXT texture;						//Texture this map uses
	GFXMATERIAL material;				//The material
	FLAGS status;						//map flags
} gfxTMap;

typedef RETCODE (* BKEFFECT) (struct _gfxBkFX *thisBKFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);

typedef struct _gfxBkFX {
	int Type;		  //Specifies effect type
	void *Data;		  //Context to which effect is applied
	BKEFFECT Effect;  //Callback function for effect
	FLAGS Status;	  //Indicates status of effect
} gfxBkFX;

//internal functions

/////////////////////////////////////
// Name:	_GFXGetD3dFormat
// Purpose:	grabs D3DFORMAT of pMode
// Output:	none
// Return:	the D3DFORMAT
/////////////////////////////////////
PROTECTED D3DFORMAT _GFXGetD3dFormat(gfxDisplayMode *pMode);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED bool _GFXCheckError(HRESULT hr, bool displayMsg, const char *header);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _TextureDestroyAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _MDLDestroyAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _OBJDestroyAll();

//md2 loader
PROTECTED RETCODE _ModelLoadDataFromFileMD2(hMDL model, const char *filename);
PROTECTED RETCODE _ModelLoadDataFromFileMS3D(hMDL model, const char *filename);

//key frame loading
PROTECTED RETCODE _ObjLoadAnimationKey(hOBJ obj, const char *animationfile);

PROTECTED void _MDLOptimize(LPD3DXMESH mesh, const LPD3DXBUFFER pAdjacencyBuffer, int numInd, 
						  LPD3DXMESH *optMesh);

//texture vtx stuff
PROTECTED void _TextureInitVtxBuff();
PROTECTED void _TextureTermVtxBuff();

//set up the billboard to face the camera
PROTECTED void TextureBillboardPrepare();

extern PUBLIC void PARFXDestroyAll();

//math stuff
//return a random double-precision number between 0 and 1
PROTECTED double _GFXMathDRand();
PROTECTED float _GFXMathRand(float a, float b);
PROTECTED void _GFXMathMtxLoadIden(gfxWrldMtx *pWMtx);
PROTECTED bool _GFXMathIsPowerOf2(int size);

//create the world matrix inside gfxWrldMtx
//wMtx->mtxWrld = scale*rot*trans
PROTECTED void _GFXCreateWrld(gfxWrldMtx *wMtx);

//pager stuff
#define GFXID_IS_EQUAL(id1, id2) ((id1).ID == (id2).ID && (id1).counter == (id2).counter)

PROTECTED void GFXPageClear();
PROTECTED void GFXPageAdd(void *item, gfxID *pID);
PROTECTED void GFXPageRemove(gfxID *pID);
PROTECTED void * GFXPageGet(DWORD ID, LONGLONG *pCounter);
PROTECTED void * GFXPageQuery(const gfxID *key);

//texture coord stuff
PROTECTED inline void _GFXVtxSetTxtCoordAll(gfxVtx * vtx, float s, float t)
{
	vtx->txt[0][eU] = vtx->txt[1][eU] = vtx->txt[2][eU] = s;
	vtx->txt[0][eV] = vtx->txt[1][eV] = vtx->txt[2][eV] = t;
	//vtx->txt[0][eW] = vtx->txt[1][eW] = vtx->txt[2][eW] = 1;
}

//frustrum

/////////////////////////////////////
// Name:	FrustrumCalculate
// Purpose:	calculate the viewing
//			frustrum
// Output:	frustrum plane calculated
// Return:	none
/////////////////////////////////////
PUBLIC void FrustrumCalculate();

// Helper function to stuff a FLOAT into a DWORD argument
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//
// Font stuff
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void FontInvalidateAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void FontRestoreAll();

//
// Texture
//

/////////////////////////////////////
// Name:	TextureInvalidateAll
// Purpose:	invalidate textures
// Output:	textures invalidated
// Return:	none
/////////////////////////////////////
PROTECTED void TextureInvalidateAll();

/////////////////////////////////////
// Name:	TextureRestoreAll
// Purpose:	restore textures
// Output:	textures restored
// Return:	none
/////////////////////////////////////
PROTECTED void TextureRestoreAll();

//
// Models
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void MDLInvalidateAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void MDLRestoreAll();

#endif
