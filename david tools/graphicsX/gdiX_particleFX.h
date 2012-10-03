#ifndef _gdix_particlefx_h
#define _gdix_particlefx_h

#include "gdix.h"

typedef struct _gfxParticleFX *hPARFX;

//return true when hit
typedef bool (* COLLISIONFUNC) (float pt1[eMaxPt], float pt2[eMaxPt], float *tOut, float nOut[eMaxPt]);

typedef enum {
	ePARFX_PUFFS,		//puffs for smoke trails and stuff burning
	ePARFX_EXPAND,		//expanding texture for cheap explosion effect
	ePARFX_EXPLODE,		//particles blast in all direction
	ePARFX_GLOW,		//one simple 'glowing' particle
	ePARFX_GAS,			//gases released within a bound box
	ePARFX_3DEXPLODE,	//3d version of explosion and then some
	ePARFX_SMOKE,		//smokey effect
	ePARFX_GATHER,		//inverse of explode
	ePARFX_LIGHTNINGY,	//'lightning' fx on y-axis
	ePARFX_MAX
} ePARFXType;

/**********************************************************
***********************************************************

  The init structs you must fill up for FX

***********************************************************
**********************************************************/

//puffy thing
typedef struct _fxPuffs_init {
	hTXT		puffTxt;			//puff image
	
	BYTE		r,g,b;				//puff color

	float		center[eMaxPt];		//used only if no obj is specified

	float		dir[eMaxPt];		//direction/velocity

	DWORD		maxParticle;		//max number of puffs

	float		scaleStart;			//starting scale
	float		scaleEnd;			//ending scale

	float		rotSpd;				//rot increment

	double		delay;				//delay of alpha fade/scale
} fxPuffs_init;

//expanding flat texture
typedef struct _fxExpand_init {
	hTXT		expandTxt;			//the image

	BYTE		r,g,b;				//color

	float		center[eMaxPt];		//used only if no obj is specified

	float		normal[eMaxPt];		//the normal (used to get the facing of particle)

	float		scaleStart;			//starting scale
	float		scaleEnd;			//ending scale

	double		delay;				//delay of scale/alpha fade (used only if the particle is not infinite)

	bool		bRepeat;			//if we want to repeat the expand effect on delay end
} fxExpand_init;

//explosion
typedef struct _fxExplode_init {
	hTXT		explodeTxt;			//the image

	BYTE		r,g,b;				//color

	float		scale;				//size of each particle (usu. 1)

	float		center[eMaxPt];		//used only if no obj is specified

	float		radius;

	DWORD		maxParticle;		//max number of puffs

	double		delay;				//delay of explosion
} fxExplode_init;

//glow
typedef struct _fxGlow_init {
	hTXT		glowTxt;			//the image

	BYTE		r,g,b;				//color

	float		scaleStart;			//starting scale
	float		scaleEnd;			//ending scale

	float		center[eMaxPt];		//used only if no obj is specified

	double		delay;				//delay of fade-in/out

	bool		bRepeat;			//if we want to repeat the fade-in/out
} fxGlow_init;

//gas
typedef struct _fxGas_init {
	hTXT		gasTxt;				//the image

	BYTE		r,g,b;				//color

	float		dir[eMaxPt];		//normalized direction
	float		spd;

	double		spawnDelay;			//delay for spawning

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	DWORD		maxParticle;		//max number of gas

	float		scaleMax;			//the maximum scale before it fades away (scale always starts at 0)

	float		scaleDelayMin;		//scale delay minimum
	float		scaleDelayMax;		//scale delay maximum

	float		alphaDelayMin;		//alpha delay minimum
	float		alphaDelayMax;		//alpha delay maximum

	bool		bRepeat;			//if we want gases to regenerate
} fxGas_init;

typedef struct _fx3DExplode_init {
	hMDL		mdl;				//the model used for explosion

	float		spd;				//speed

	float		center[eMaxPt];		//used only if no obj is specified

	float		dir[eMaxPt];		//direction/velocity going down
	float		dirAcc[eMaxPt];		//direction acceleration

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	DWORD		maxParticle;		//max number of exploding mdl.

	double		delay;				//delay of fade
} fx3DExplode_init;

//smoke
typedef struct _fxSmoke_init {
	hTXT		smokeTxt;			//the image

	BYTE		r,g,b;				//color

	double		spawnDelay;			//delay for spawning
	double		spdDelay;			//speed going from start to end
	double		fadeDelay;			//delay for fading away

	float		size;				//size of each particle

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	float		sLoc[eMaxPt];		//the starting location if no joint index specified
	float		eLoc[eMaxPt];		//the ending location if no end joint index specified

	int			eJointInd;			//the ending joint index, this is used if not -1

	DWORD		maxParticle;		//max number of smoke

	int			axis;				//which axis to set straight (0 = x, 1 = y, 2 = z), -1 = none

	bool		bRepeat;			//if we want smokes to regenerate
} fxSmoke_init;

//Gather
typedef struct _fxGather_init {
	hTXT		explodeTxt;			//the image

	BYTE		r,g,b;				//color

	float		scale;				//size of each particle (usu. 1)

	float		center[eMaxPt];		//used only if no obj is specified

	float		radius;

	DWORD		maxParticle;		//max number of puffs

	double		delayGather;		//delay of gathering
	double		delayFade;			//delay of fade after reaching center
	double		delaySpawn;			//delay of spawn
} fxGather_init;

//Lightning Y
typedef struct _fxLightningY_init {
	hTXT		lightningTxt;		//the image

	BYTE		r,g,b;				//color

	float		scale;				//size of each particle (usu. 1)

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	DWORD		maxParticle;		//max number of gas

	DWORD		maxPoint;			//max number of points along Y

	double		delayOn;			//delay of turning 'ON'
	double		delayMove;			//delay of movement
} fxLightningY_init;


/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hPARFX PARFXCreate(ePARFXType type, void *initStruct, double durDelay, 
						  hOBJ objAttach, int objJointInd, COLLISIONFUNC collision);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void PARFXDestroyAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void PARFXDestroy(hPARFX *parFX);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdate(hPARFX parFX);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplay(hPARFX parFX);

/////////////////////////////////////
// Name:	PARFXUpdateAll
// Purpose:	updates all particles
//			created.
//			NOTE: This will destroy
//				  any particle that has
//				  expired
// Output:	stuff updated
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdateAll();

/////////////////////////////////////
// Name:	PARFXDisplayAll
// Purpose:	displays all particles
//			created.
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplayAll();

/////////////////////////////////////
// Name:	PARFXActivate
// Purpose:	activate/deactivate given
//			particle
// Output:	particle deactivated
// Return:	none
/////////////////////////////////////
PUBLIC void PARFXActivate(hPARFX parFX, bool bActive);

/////////////////////////////////////
// Name:	PARFXIsActive
// Purpose:	check to see if the given
//			particle FX is active
// Output:	none
// Return:	true if active
/////////////////////////////////////
PUBLIC bool PARFXIsActive(hPARFX parFX);

#endif