#ifndef _gdix_particlefx_i_h
#define _gdix_particlefx_i_h

#include "gdix_particlefx.h"

#define PARFX_UPDATE_DEAD	1

//flags for particle
#define PARFX_FLAG_INFINITE			1
#define PARFX_FLAG_OBJATTACHED		2
#define PARFX_FLAG_DONE				4
#define PARFX_FLAG_DISABLE			8	//particle FX will stop updating/display
#define PARFX_FLAG_DISABLE_DONE		16	//particle completely disabled

//message for FX functions
typedef enum {
	PARFXM_CREATE,
	PARFXM_UPDATE,	//LPARAM == PARFX_UPDATE_DEAD, dead
	PARFXM_DISPLAY,
	PARFXM_DESTROY
} FXmessage;

/**********************************************************
***********************************************************

  Particle FX internal functions

***********************************************************
**********************************************************/

//get object
PROTECTED hOBJ PARFXGetOBJ(hPARFX parFX);

//get obj loc
PROTECTED void PARFXGetOBJLoc(hPARFX parFX, hOBJ obj, float ptOut[eMaxPt]);

//check status
PROTECTED bool PARFXCheckStatus(hPARFX parFX, DWORD status);

//return true when hit
PROTECTED bool PARFXCollision(hPARFX parFX, float pt1[eMaxPt], float pt2[eMaxPt], float *tOut, float nOut[eMaxPt]);

PROTECTED void PARFXPrepare();
PROTECTED void PARFXUnPrepare();

/**********************************************************
***********************************************************

  Data structs for Particle FXs

***********************************************************
**********************************************************/

/**********************************************************
	Main FX
**********************************************************/
typedef RETCODE (* PAREFFECT) (struct _gfxParticleFX *thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);

typedef struct _gfxParticleFX {
	int type;			//specifies particle type
	void *data;			//contect to which effect is applied
	PAREFFECT Effect;   //Callback function for effect
	
	win32Time ticker;	//timer for duration of the particle

	FLAGS status;		//indicates status of effect

	gfxID objKey;		//reference to object attached
	int	  objJointInd;	//object joint

	COLLISIONFUNC collision;

} gfxParticleFX;

/**********************************************************
	FX Puffs
**********************************************************/
typedef struct _fxPuff_par {
	float		loc[eMaxPt];

	float		clr[eMaxClr];		//color

	float		scale;				//current scale

	float		rot;				//current rotate

	win32Time	delayTimer;			//alpha/scale timer
} fxPuff_par;

typedef list<fxPuff_par> fxPuffList;

typedef struct _fxPuffs {
	hTXT		puffTxt;			//puff image

	float		r,g,b;				//puff color (0~1)

	float		center[eMaxPt];		//used only if no obj is specified

	float		dir[eMaxPt];		//direction/velocity

	DWORD		maxParticle;		//max number of puffs

	float		scaleStart;			//starting scale
	float		scaleEnd;			//ending scale

	float		rotSpd;				//rot increment

	double		delay;				//delay for alpha and scale

	fxPuffList	*puffs;				//list of puffs
} fxPuffs;

/**********************************************************
	Expand Thing
**********************************************************/
typedef struct _fxExpand {
	hTXT		expandTxt;			//the image

	BYTE		r,g,b;

	DWORD		clr;				//color

	float		scale;				//current scale

	float		center[eMaxPt];		//used only if no obj is specified

	float		scaleStart;			//starting scale
	float		scaleEnd;			//ending scale

	GFXVTXBUFF  *vtxBuff; //the four vtx pt.

	win32Time	delayTimer;			//timer

	bool		bRepeat;			//if we want to repeat the expand effect on delay end
} fxExpand;

/**********************************************************
	Explosion
**********************************************************/
typedef struct _fxExplodePar {
	float		loc[eMaxPt];
	float		endLoc[eMaxPt];
} fxExplodePar;

typedef struct _fxExplode {
	hTXT		explodeTxt;			//the image

	float		center[eMaxPt];		//used only if no obj is specified

	float		clr[eMaxClr];		//color

	float		scale;				//size of each particle (usu. 1)

	fxExplodePar *particles;		//the explosion particles
	DWORD		maxParticle;		//max number of puffs

	win32Time	delayTimer;			//timer
} fxExplode;

/**********************************************************
	Glow
**********************************************************/
typedef struct _fxGlow {
	hTXT		glowTxt;			//the image

	float		clr[eMaxClr];		//color

	float		alphaStart;			//starting alpha
	float		alphaEnd;			//ending alpha

	float		scale;				//the current scale

	float		scaleStart;			//starting scale
	float		scaleEnd;			//ending scale

	float		center[eMaxPt];		//used only if no obj is specified

	win32Time	delayTimer;			//timer

	bool		bRepeat;			//if we want to repeat the fade-in/out
} fxGlow;

/**********************************************************
	Gas
**********************************************************/
typedef struct _fxGas_par {
	float		loc[eMaxPt];

	float		clr[eMaxClr];		//color

	float		scale;				//current scale

	bool		bFading;			//are we fading?

	win32Time	delayTimer;			//scale/alpha timer
} fxGas_par;

typedef list<fxGas_par> fxGasList;

typedef struct _fxGas {
	hTXT		gasTxt;				//the image

	float		r,g,b;				//color

	float		dir[eMaxPt];		//normalized direction
	float		spd;

	win32Time	spawnDelay;			//spawn delay timer

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	DWORD		maxParticle;		//max number of gas

	float		scaleMax;			//the maximum scale before it fades away (scale always starts at 0)

	float		scaleDelayMin;		//scale delay minimum
	float		scaleDelayMax;		//scale delay maximum

	float		alphaDelayMin;		//alpha delay minimum
	float		alphaDelayMax;		//alpha delay maximum

	fxGasList	*gases;				//the gas particle list

	bool		bRepeat;			//if we want gases to regenerate
} fxGas;

/**********************************************************
	3D Explode
**********************************************************/
typedef struct _fx3DExplodePar {
	hOBJ	    obj;				//the explosion chunk
	float		orgLoc[eMaxPt];		//origin
	float		dir[eMaxPt];		//normalized direction
	float		xDir[eMaxPt];		//the other dir
} fx3DExplodePar;

typedef struct _fx3DExplode {
	float		spd;				//speed

	float		dir[eMaxPt];		//direction/velocity going down
	float		dirAcc[eMaxPt];		//direction acceleration

	fx3DExplodePar *particles;		//the explosion chunks
	DWORD		    maxParticle;	//max number of exploding mdl.

	win32Time	delayTimer;			//timer
} fx3DExplode;

/**********************************************************
	Smoke
**********************************************************/
typedef struct _fxSmoke_par {
	float		loc[eMaxPt];

	float		sLoc[eMaxPt];
	float		eLoc[eMaxPt];

	float		clr[eMaxClr];		//color

	bool		bFading;			//are we fading?

	win32Time	delayTimer;			//fade timer
} fxSmoke_par;

typedef list<fxSmoke_par> fxSmokeList;

typedef struct _fxSmoke {
	hTXT		smokeTxt;				//the image

	float		r,g,b;				//color

	win32Time	spawnDelay;			//spawn delay timer
	double		spdDelay;			//speed going from start to end
	double		fadeDelay;			//fade delay value

	float		size;				//size of particle

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	float		sLoc[eMaxPt];		//the starting location if no joint index specified
	float		eLoc[eMaxPt];		//the ending location if no end joint index specified

	int			eJointInd;			//the ending joint index, this is used if not -1

	DWORD		maxParticle;		//max number of smoke

	int			axis;				//which axis to set straight (0 = x, 1 = y, 2 = z), -1 = none

	fxSmokeList	*smokes;			//the smoke particle list

	bool		bRepeat;			//if we want smokes to regenerate
} fxSmoke;

/**********************************************************
	Gather
**********************************************************/
typedef struct _fxGatherPar {
	float		loc[eMaxPt];		//current loc.

	float		sLoc[eMaxPt];
	float		eLoc[eMaxPt];

	float		scale;				//current scale
	
	float		clr[eMaxClr];		//color

	bool		bFading;			//are we fading?

	win32Time	delayTimer;			//gather/fade
} fxGatherPar;

typedef list<fxGatherPar> fxGatherList;

typedef struct _fxGather {
	hTXT		explodeTxt;			//the image

	float		center[eMaxPt];		//used only if no obj is specified

	float		clr[eMaxClr];		//color

	float		scale;				//size of each particle (usu. 1)

	float		radius;				//the radius of the gathering area

	fxGatherList *particles;		//the explosion particles

	DWORD		maxParticle;		//max number of puffs

	double		delayFade;			//fade delay value
	double		delayGather;		//gather delay value

	win32Time	delaySpawn;			//spawn delay timer

} fxGather;

/**********************************************************
	Lightning Y-axis
**********************************************************/
typedef struct _lightningNode {
	D3DXVECTOR3	sLoc;
	D3DXVECTOR3	eLoc;

	D3DXVECTOR3	curLoc;

	win32Time	timeMove;
} lightningNode;

typedef struct _fxLightningY {
	hTXT		lightningTxt;		//the image

	float		clr[eMaxClr];		//color

	float		scale;				//size of each particle (usu. 1)

	float		min[eMaxPt];		//minimum bbox spawn area (relative to obj, if attached)
	float		max[eMaxPt];		//maximum bbox spawn area (relative to obj, if attached)

	DWORD		maxParticle;		//max number of puffs

	lightningNode *Nodes;	//array of lightning nodes
	DWORD		numNodes;

	double		delayMove;			//delay move

	win32Time	onTimer;			//the 'on' timer
} fxLightningY;

//func.
PROTECTED RETCODE fxPuffFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxExpandFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxExplodeFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxGlowFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxGasFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fx3DExplodeFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxSmokeFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxGatherFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxLightningYFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);

#endif