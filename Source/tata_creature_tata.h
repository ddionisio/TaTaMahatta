#ifndef _tata_creature_tata_h
#define _tata_creature_tata_h

#include "tata_creature.h"

//This includes all Ta-Ta's

class TaTaCommon : public Creature {
public:
	TaTaCommon(int subType) : Creature(ENTITY_TYPE_TATA, subType), m_portraitTxt(0), m_attachedOBJ(0) {}
	virtual ~TaTaCommon();

	virtual void MoveAnim(eMoveType type);

	//Collide Entity, Ta-Ta style
	void CollideEntity(EntityCommon *pEntity, const gfxTrace & trace);
	
	//Collide World, Ta-Ta style
	void CollideWorld(hQBSP qbsp, const gfxTrace & trace);

	//set the Ta-Ta portrait
	void SetPortrait(hTXT newPort);

	//get portrait (DON'T DELETE!!!)
	hTXT GetPortrait();

	//attaches given Ta-Ta (only use with special Ta-Ta NPC (BabyTaTa))
	//NOTE: this will destroy the entity except for the object.
	void AttachTaTa(TaTaCommon *pTata, int hugState);

	//detaches the Ta-Ta and sets attach object to 0,
	//after calling this, you must destroy the returned
	//object manually!!!
	//you can optionally get the spawn loc if you want
	hOBJ DetachTaTa(D3DXVECTOR3 *pSpawnLoc = 0);

	//check to see if there is someone attached to us
	bool IsAttach();

	int GetOuchState() { return CREANIM_OUCH; }
	int GetDeathState() { return CREANIM_DEATH; }

	const char *GetSubTypeStr() const;

protected:
	hTXT  m_portraitTxt;		//the portrait for this Ta-Ta

	//for rescue missions
	hOBJ  m_attachedOBJ;			//the attached object
	D3DXVECTOR3 m_attachedSpawnLoc;	//just in case we need to respawn the attached Ta-Ta
};

/////////////////////////////////////////////////////////////////////////////
// Captain Ta-Ta
/////////////////////////////////////////////////////////////////////////////
class CaptainTaTa : public TaTaCommon {
public:
	CaptainTaTa() : TaTaCommon(TATA_CAPTAINTATA), m_blockState(0), m_blockStandState(0), m_blockMoveState(0), m_normSpd(0), m_blockSpd(0) {}
	~CaptainTaTa();

	void InputRotate();	//rotate by INP_LEFT or INP_RIGHT
	void InputStrafe();  //strafe
	void InputWalk();	//move forward by INP_UP or INP_DOWN

	void MoveAnim(eMoveType type);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int m_blockState;			//block preparation animation state
	int m_blockStandState;		//block standing animation state
	int m_blockMoveState;		//block moving animation state
	float m_normSpd;			//temporary holder for normal speed
	float m_blockSpd;			//speed of movement while blocking
};

/////////////////////////////////////////////////////////////////////////////
// Ta-Ta Tug
/////////////////////////////////////////////////////////////////////////////
class TaTaTug : public TaTaCommon {
public:
	TaTaTug() : TaTaCommon(TATA_TATATUG), m_pushState(0), m_pushSpd(0) {}
	~TaTaTug();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int   m_pushState;			//push animation state
	float m_pushSpd;			//spd data for push melee projectile
};

/////////////////////////////////////////////////////////////////////////////
// Tinker Ta
/////////////////////////////////////////////////////////////////////////////
class TinkerTa : public TaTaCommon {
public:
	TinkerTa() : TaTaCommon(TATA_TINKERTA), m_useState(0), m_useBound(0,0,0) {}
	~TinkerTa();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int   m_useState;			//use animation state
	D3DXVECTOR3 m_useBound;		//the use bound
};

/////////////////////////////////////////////////////////////////////////////
// Ta-Ta Trample
/////////////////////////////////////////////////////////////////////////////
class TaTaTrample : public TaTaCommon {
public:
	TaTaTrample() : TaTaCommon(TATA_TATATRAMPLE), m_attackPrepState(0), m_attackState(0), m_attackBound(0,0,0) {}
	~TaTaTrample();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_attackPrepState;		//prepare attack animation state
	int				m_attackState;			//attack animation state
	int				m_attack2PrepState;		//prepare attack animation state 2
	int				m_attack2State;			//attack animation state 2
	D3DXVECTOR3		m_attackBound;			//the max attack bound
};

/////////////////////////////////////////////////////////////////////////////
// Hop-Ta-Hop
/////////////////////////////////////////////////////////////////////////////
class HopTaHop : public TaTaCommon {
public:
	HopTaHop() : TaTaCommon(TATA_HOPTAHOP), m_jumpState(0), m_bJump(false) {}
	~HopTaHop();

	void InputStrafe();  //strafe

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_jumpState;			//jump animation state
	Move			m_jumpSpd;				//the jump speed
	bool			m_bJump;				//jumping (if true, we can do air control)
};

/////////////////////////////////////////////////////////////////////////////
// Para Ta-Ta
/////////////////////////////////////////////////////////////////////////////
class ParaTaTa : public TaTaCommon {
public:
	ParaTaTa();
	~ParaTaTa();

	void InputWalk();	//move forward by INP_UP or INP_DOWN

	void InputStrafe();  //strafe

	void MoveAnim(eMoveType type);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_glideState;			//gliding animation state
	Move			m_glideSpd;				//the jump speed
	bool			m_bGlide;				//are we gliding?
};

/////////////////////////////////////////////////////////////////////////////
// Loop-Ta-Loop
/////////////////////////////////////////////////////////////////////////////
class LoopTaLoop : public TaTaCommon {
public:
	LoopTaLoop();
	~LoopTaLoop();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_attackState;			//whip attack animation state
	int				m_attackEndState;		//whip stretching
	int				m_grappleState;			//grappling animation state
	Move			m_whipSpd;				//the whip speed
	float			m_whipLen;				//accumulated whip length
	float			m_whipLenMax;			//maximum whip length
	D3DXVECTOR3		m_attackBound;			//the max attack bound
	Id				m_proj;					//the whip projectile
	double			m_stun;					//stun delay
};

/////////////////////////////////////////////////////////////////////////////
// Frost-Ta
/////////////////////////////////////////////////////////////////////////////
class FrostTa : public TaTaCommon {
public:
	FrostTa();
	~FrostTa();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_attackState;			//gun attack animation state
	D3DXVECTOR3		m_attackBound;			//the max attack bound
	hTXT			m_frostFXTxt;			//frost gas texture
	BYTE			m_r,m_g,m_b;			//color of frost gas

	hTXT			m_frozenTxt;			//the texture used when freezing an enemy
	hMDL			m_frozenMdl;			//the frozen chunk when block is destroyed

	Id				m_proj;					//the frost projectile

	HCHANNEL		m_frostChan;			//frost sound channel
};

/////////////////////////////////////////////////////////////////////////////
// Key-Key-Ta
/////////////////////////////////////////////////////////////////////////////
class KeyKeyTa : public TaTaCommon {
public:
	KeyKeyTa();
	~KeyKeyTa();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	void OpenTheDoor();	//open the god damn door

private:
	int				m_useState;		//animation for 'lockpicking'
	int				m_climbState;	//animation for climbing
	Move			m_climbSpd;		//speed of climbing
	bool			m_bClimb;		//if true, we are climbing
	bool			m_bCeiling;		//we hit the ceiling
	bool			m_bCanClimb;	//are we permitted to climb?

	HCHANNEL		m_climbSnd;		//the climb sound
};

/////////////////////////////////////////////////////////////////////////////
// Chi-Ta
/////////////////////////////////////////////////////////////////////////////
class ChiTa : public TaTaCommon {
public:
	ChiTa();
	~ChiTa();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_pullState;			//pull animation state
	D3DXVECTOR3		m_projBound;			//the max projectile bound
	float			m_pullSpd;				//the pull speed
	float			m_pullDist;				//the pull distance

	Id				m_projID;				//the projectile ID

	HCHANNEL		m_pullSnd;				//the pull sound
};

/////////////////////////////////////////////////////////////////////////////
// TaVatar
/////////////////////////////////////////////////////////////////////////////
class TaVatar : public TaTaCommon {
public:
	TaVatar();
	~TaVatar();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int			m_atkPrepState;			//attack prep. animation state
	int			m_atkState;				//attack animation state

	hMDL		m_fBallMdl;			//the model of the fireball
	hTXT		m_fBallFXTxt;		//the fireball particle FX texture
	float		m_fBallRadius;		//the fireball radius
	BYTE		m_r,m_g,m_b;		//particle color
	
	float		m_attackSpd;		//the speed of the projectile

	bool		m_bChargeHasEnd;	//has the charging ended?
	bool		m_bFire;			//throw the fireball?

	hPARFX		m_fxCharge;				//the charge FX while preparing to attack
};

/////////////////////////////////////////////////////////////////////////////
// Baby Ta-Ta
/////////////////////////////////////////////////////////////////////////////
class BabyTaTa : public TaTaCommon {
public:
	BabyTaTa() : TaTaCommon(TATA_BABYTATA), m_hugState(0) {}
	~BabyTaTa();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int   m_hugState;			//the state when this Ta-Ta will be carried
};

#endif