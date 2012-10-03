#ifndef _gdix_types_h
#define _gdix_types_h

//Internal Use Only!!!

//wrapper typedef so that we can upgrade easily
typedef LPDIRECT3D9						PGFXMAIN;
typedef LPDIRECT3DDEVICE9				PGFXDEVICE;
typedef D3DCAPS9						GFXCAPS;
typedef LPDIRECT3DTEXTURE9				PGFXTEXTURE;
typedef IDirect3DIndexBuffer9			GFXINDBUFF;
typedef IDirect3DVertexBuffer9			GFXVTXBUFF;
typedef LPDIRECT3DVERTEXSHADER9			PGFXVERTEXSHADER;
typedef LPDIRECT3DVERTEXDECLARATION9	PGFXVERTEXDECLARATION;

//constants and macros
#define FOV (D3DX_PI/4)//(D3DX_PI/2)//

#define MDLTXTID		 90210		  //It's the constant model texture ID
#define AMBIENT_DEFAULT  0xFFFFFFFF   //default ambient light
#define MILLISECOND		 1000		  //millisecond speed (used in timer)
#define FRAMEMAXCHAR	 16			  //used by MD2
#define SINCOUNT		 4096		  //Used for Sin/Cos table
#define NAMEMAX			 256
#define NAMEMAX2		 32
#define MAXLIGHT		 8

#define GFX_TIME_TOLER	 0.001

#define SIN(x) g_sinTable[(((x)*SINCOUNT)/360)&(SINCOUNT-1)]
#define COS(x) g_sinTable[((((x)+90)*SINCOUNT)/360)&(SINCOUNT-1)]

#define GFXVERTEXTEXTUREFLAG (D3DFVF_TEX3)

#define GFXVERTEXFLAG (D3DFVF_XYZ | D3DFVF_NORMAL | GFXVERTEXTEXTUREFLAG) //xyz and color and texture

#define kDEPTHNEAR		 1//0.5f
#define kDEPTHFAR		 1000.0f

//flags for GFX
#define GFX_FULLSCREEN		1			  //are we in fullscreen?
#define GFX_BLTMODE			2			  //are we in blt mode?
#define GFX_LIGHTDISABLE	4			  //is light disabled?
#define GFX_SCREENCAPTURE	8			  //capturing screen

//flags for models
#define MDL_BACKFACECCW	 1			  //does the model use CCW culling?
#define MDL_YDEPTH		 2			  //Y is depth, we need to rotate X-axis by -90
#define MDL_JOINT		 4			  //Model is using joints (joint frames)
#define MDL_FRAME		 8			  //Model is using frames (key frame vertices)

//flags for Objects
#define OBJ_CHILD			1		  //object is a child
#define OBJ_SYNCSTATE		2		  //object's state is in sync with parent
#define OBJ_DEACTIVATE		4		  //object is deactivated
#define OBJ_PAUSE			8		  //object is paused (no frame update)
#define OBJ_PAUSE_ON_END	16		  //object will pause on state end frame

//flags for sprites
#define SPR_PAUSE			1		  //object is paused (no frame update)
#define SPR_PAUSE_ON_END	2		  //object will pause on state end frame

//flags for Terrain Map
#define TMAP_DRAWLINE		1		  //will draw terrain in wire frame

//typedefs
typedef list<unsigned int> LISTPTR;	  //list of pointers

#endif