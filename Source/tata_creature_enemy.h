#ifndef _tata_creature_enemy_h
#define _tata_creature_enemy_h

#include "tata_creature.h"

//This includes all Enemies

class EnemyCommon : public Creature {
public:
	EnemyCommon(int subType) : Creature(ENTITY_TYPE_ENEMY, subType) {}
	virtual ~EnemyCommon() {}

	const char *GetSubTypeStr() const;

	//Collide Entity, Enemy style
	void CollideEntity(EntityCommon *pEntity, const gfxTrace & trace);
	
	//Collide World, Enemy style
	void CollideWorld(hQBSP qbsp, const gfxTrace & trace);

protected:
};

/////////////////////////////////////////////////////////////////////////////
// Funga Bark
/////////////////////////////////////////////////////////////////////////////
class FungaBark : public EnemyCommon {
public:
	FungaBark();
	~FungaBark();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

private:
	float		m_attackRadiusSQ;		//attack radius^2 (any Ta-Ta who gets within this radius will be attacked)
	D3DXVECTOR3 m_attackBound;

	HCHANNEL	m_growlSnd;				//the growling sound
};

/////////////////////////////////////////////////////////////////////////////
// Shroom Shooter
/////////////////////////////////////////////////////////////////////////////
class ShroomShooter : public EnemyCommon {
public:
	ShroomShooter();
	~ShroomShooter();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

private:
	hMDL		m_spitMdl;			//the model of the projectile
	hTXT		m_spitFXTxt;		//the spit particle FX texture
	BYTE		m_r,m_g,m_b;		//particle color
	
	float		m_attackSpd;		//the speed of the projectile
	win32Time	m_attackDelay;		//delay before spitting again
};

/////////////////////////////////////////////////////////////////////////////
// Shroom Guard
/////////////////////////////////////////////////////////////////////////////
class ShroomGuard : public EnemyCommon {
public:
	ShroomGuard();
	~ShroomGuard();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void MoveAnim(eMoveType type);

private:
	float		m_attackRadiusSQ;		//attack radius^2 (any Ta-Ta who gets within this radius will be attacked)
	float		m_followRadiusSQ;		//follow radius^2 (follow any Ta-Ta who gets within this radius)
	D3DXVECTOR3 m_attackBound;
	D3DXVECTOR3 m_attackDir;			//when attacking

	HCHANNEL	m_followSnd;			//the sound makes when a tata is detected
};

/////////////////////////////////////////////////////////////////////////////
// Funga Spy
/////////////////////////////////////////////////////////////////////////////
class FungaSpy : public EnemyCommon {
public:
	FungaSpy();
	~FungaSpy();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void SetAngleScan(float radian);
	float GetAngleScan();

	void SetLengthScan(float l);
	float GetLengthScan();

	void SetScanDelay(double delay);

private:
	float		m_angleScan;	//max angle range of scan [-angle, angle], in radians
	float		m_lengthScan;	//length of sight

	bool		m_bScanRangeInit;		//did we initialized the scan angle range?
	float		m_angleMin,m_angleMax;	//initialized if m_bScanRangeInit is false
	float		m_angleAccum;			//accumulated angle

	D3DXVECTOR3 m_origDir;		//the direction of funga spy upon loading

	float		m_rotDir;		//-1 or 1, changes when angle reaches min or max

	hMDL		m_spikeMDL;
	float		m_spikeSpd;

	bool		m_bWait;		//are we waiting?
	win32Time	m_scanDelay;	//pause amount when end reached

	Id			m_scannerID;	//the scanner ID projectile
};

/////////////////////////////////////////////////////////////////////////////
// Funga Smug
/////////////////////////////////////////////////////////////////////////////
class FungaSmug : public EnemyCommon {
public:
	FungaSmug();
	~FungaSmug();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void MoveAnim(eMoveType type);

private:
	float			m_attackRadiusSQ;		//attack radius^2 (any Ta-Ta who gets within this radius will be attacked)
	D3DXVECTOR3		m_attackBound;
	Id				m_gasID;				//gas attack

	hTXT			m_gasTxt;				//gas FX texture
	float			m_gasSize;				//gas particle size
	BYTE			m_r,m_g, m_b;			//gas clr.
};

/////////////////////////////////////////////////////////////////////////////
// Funga Musketeer
/////////////////////////////////////////////////////////////////////////////
class FungaMusketeer : public EnemyCommon {
public:
	FungaMusketeer();
	~FungaMusketeer();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void MoveAnim(eMoveType type);

private:
	float		m_attackRadiusSQ;		//attack radius^2 (any Ta-Ta who gets within this radius will be attacked)
	float		m_bulletSpd;			//speed of bullet

	hMDL		m_bulletMdl;			//the model of the projectile
	hTXT		m_bulletFXTxt;			//the bullet particle FX texture
	BYTE		m_r,m_g,m_b;			//particle color

	win32Time	m_blockDelay;		    //pause amount when blocking

	D3DXVECTOR3 m_fixedDir;				//fixed direction when firing and blocking
};

//
// Bosses
//

/////////////////////////////////////////////////////////////////////////////
// Shroom Pa
/////////////////////////////////////////////////////////////////////////////
class ShroomPa : public EnemyCommon {
public:
	enum {
		SHROOMPA_STATE_WANDER,	//wandering around
		SHROOMPA_STATE_CHARGE,	//charge!!!!
		SHROOMPA_STATE_SPIT,	//SPIT!!!
		SHROOMPA_STATE_DIZZY,	//dizzy...
		SHROOMPA_STATE_MAX
	};

	ShroomPa();
	~ShroomPa();

	void RandomChanceOfCrap();
	void ChangeBehavior(int state);

	void BehaviorUpdate(bool bBegin);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void MoveAnim(eMoveType type);

private:
	int m_actionState;						//current state: SHROOMPA_STATE_*
	int m_actionChance[SHROOMPA_STATE_MAX]; //chances to do specific action...randomly determined
											//-1 means no chance

	win32Time   m_actionDelay;		//delay timer to change action

	win32Time	m_dizzyDelay;		//delay timer for dizziness

	win32Time	m_dizzyAfterHitDelay; //the delay timer for dizziness after hit

	bool		m_bDizzyAfterHit;	  //did we get hit while being dizzy?

	D3DXVECTOR3 m_dest;				//destination loc. (used after charging)
	bool		m_bGoToDest;		//want to go back to destination?

	BYTE		m_r,m_g,m_b;		//particle color

	hMDL		m_spitMdl;			//the model of the projectile
	hTXT		m_spitFXTxt;		//the spit particle FX texture

	float		m_spitAngleStart;	//relative angle start for spit (in radians)
	float		m_spitAngleInc;		//angle increment (in radians)

	int			m_spitMax;			//max number of spit
	int			m_spitCur;			//current number of spit made

	float		m_spitSpd;			//speed of spit

	float		m_normSpd;			//temporary holder for normal speed
	float		m_chargeSpd;		//speed of movement while charging
	
	win32Time   m_chargeDelay;		//delay of charge before going back to walk

	D3DXVECTOR3 m_chargeDir;		//charing direction

	hPARFX		m_particle;			//particle FX for dust smoke crap
};

/////////////////////////////////////////////////////////////////////////////
// Catter Shroom
/////////////////////////////////////////////////////////////////////////////
class CatterShroom : public EnemyCommon {
public:
	enum {
		CATTERSHROOM_STATE_IDLE,			//catter shroom is just standing
		CATTERSHROOM_STATE_ATTACK_PATTERN,
		CATTERSHROOM_STATE_SPIT,			//catter shroom machine gun spit
		CATTERSHROOM_STATE_MUCUS,			//catter shroom throws a big mucus
											//upward and lands on selected Ta-Ta
		CATTERSHROOM_STATE_ATTACK,			//the ground spike thingy
		CATTERSHROOM_STATE_STUCK,			//arm stuck on the ground after attack
		CATTERSHROOM_STATE_DIG,				//catter shroom digging
		CATTERSHROOM_STATE_MAX
	};

	CatterShroom();
	~CatterShroom();

	void ChangeBehavior(int state);

	void BehaviorUpdate(bool bBegin);

	void GasActivate(bool bYes);		//activate gas?
	
	void FFieldActivate(bool bYes);		//activate fire force field?
	void FFieldUpdate();				//update force field for fire
	bool FFieldIsActive();				//is force field active

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

	void MoveAnim(eMoveType type);

private:
	int m_actionState;						//current state: CATTERSHROOM_STATE_*

	//////////////////////////////////////////
	// Spit
	hMDL		m_spitMdl;			//the model of the projectile
	hTXT		m_spitFXTxt;		//the spit particle FX texture

	float		m_spitAngleMin;		//random angle min
	float		m_spitAngleMax;		//random angle max

	int			m_spitMax;			//max number of spit
	int			m_spitCur;			//current number of spit made

	float		m_spitSpd;			//speed of spit

	win32Time	m_spitDelay;		//delay of each spit

	//////////////////////////////////////////
	// Big Mucus
	hMDL		m_mucusMdl;			//the model of the mucus
	hTXT		m_mucusFXTxt;		//the mucus particle FX texture
	float		m_mucusSpd;			//speed of mucus

	Id			m_mucusID;			//the mucus projectile

	//////////////////////////////////////////
	// Spike
	hMDL		m_spikeMdl;			//the spike model
	float		m_spikeSpd;			//the spike speed

	int			m_spikeMax;			//max number of spike
	int			m_spikeCur;			//current number of spike made

	win32Time	m_spikeDelay;		//delay for next spike

	D3DXVECTOR3 m_spikeTarget;		//the target of the spike
	D3DXVECTOR3 m_spikeVec;			//the vector of the spike
	float		m_spikeStartY;		//the start Y of spike
	float		m_spikeEndY;		//the end Y of spike

	//////////////////////////////////////////
	// Fire Force Field
	hOBJ		m_ffieldOBJ;		//the force field object
	Move		m_ffieldRotSpd;		//force field rotation speed
	hTXT		m_ffieldTxtFX;		//force field explosion FX texture
	bool		m_bFfieldOn;		//is force field on? (only set by FFieldActivate)

	//////////////////////////////////////////
	// Gaseous Area
	Id			m_gasID;			//gas attack

	hTXT		m_gasTxt;			//gas FX texture
	float		m_gasSize;			//gas particle size

	D3DXVECTOR3	m_gasBound;			//the gas bound

	//////////////////////////////////////////
	// Misc.
	float		m_attackRadiusSQ;	//range of attack
	win32Time	m_stuckDelay;		//delay after spike attacks
	win32Time	m_idleDelay;		//idle delay
	win32Time	m_patternDelay;		//pattern delay (pattern attack)

	vector< vector< int > > m_patternAtk;	//attack patterns [hp][pattern]
	int			m_patternCur;				//current pattern

	vector< D3DXVECTOR3 > m_digLocs;		//digging locations (MaxHP - 1)

	bool		m_bTataInRange;		//is Ta-Ta in range?
	int			m_patternInd;		//index pattern of patterns
};

/////////////////////////////////////////////////////////////////////////////
// Corrup-Ta
/////////////////////////////////////////////////////////////////////////////
class CorrupTa : public EnemyCommon {
public:
	enum {
		CORRUPTA_STATE_IDLE_1,
		CORRUPTA_STATE_IDLE_2,
		CORRUPTA_STATE_METEOR_1,
		CORRUPTA_STATE_METEOR_2,
		CORRUPTA_STATE_TATAMAHATTA,
		CORRUPTA_STATE_TELEPORT,
		CORRUPTA_STATE_PATTERN_ATTACK,
		CORRUPTA_STATE_FIREBALL,
		CORRUPTA_STATE_SUCKERBALL,
		CORRUPTA_STATE_MAX
	};

	CorrupTa();
	~CorrupTa();

	void TeleportME();

	void GenerateBlast(Creature *pCre, double delay);	//creates a blast aura at bottom of creature

	void GasActivate(bool bYes);		//activate gas?
	
	void FFieldActivate(bool bYes);		//activate fire force field?
	void FFieldUpdate();				//update force field for fire
	bool FFieldIsActive();				//is force field active

	void ChangeBehavior(int state);

	void BehaviorUpdate(bool bBegin);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	int GetDeathState();
	int GetOuchState();

private:
	int m_actionState;						//current state: CORRUPTA_STATE_*

	hPARFX		m_fxCharge;					//the charge FX while preparing to attack

	win32Time	m_idleDelay;				//idle delay
	win32Time	m_patternDelay;				//pattern delay (pattern attack)

	vector< vector< int > > m_patternAtk;	//attack patterns [hp][pattern]
	int			m_patternCur;				//current pattern

	int			m_patternInd;				//index pattern of patterns

	bool		m_bGenerateFField;			//do you want to regenerate force field?

	hTXT		m_blastFXTxt;				//used by meteor

	//////////////////////////////////////////
	// Fireball
	hMDL		m_ballMdl;			//the model of the projectile
	hTXT		m_ballFXTxt;		//the fireball particle FX texture

	float		m_ballAngleMin;		//random angle min
	float		m_ballAngleMax;		//random angle max

	int			m_ballMax;			//max number of fireball
	int			m_ballCur;			//current number of fireball made

	float		m_ballSpd;			//speed of meteor

	float		m_ballRadius;		//the radius of the ball

	int			m_ballNumBounce;	//number of bounces the ball makes

	win32Time	m_ballDelay;		//delay of each fireball
	win32Time	m_ballChargeDelay;	//delay of charging

	//////////////////////////////////////////
	// Sucker Ball
	Move			m_pullMov;				//the pull movement
	win32Time		m_pullDelay;			//delay of pull
	HCHANNEL		m_suckSnd;				//the sucking sound

	//////////////////////////////////////////
	// Meteor 1 & 2
	hTXT		m_meteorFXTxt;		//the meteor 1&2 particle FX texture

	float		m_meteorSpd;		//speed of meteor
	float		m_meteorRadius;		//the radius of the meteor
	win32Time	m_meteorDelay;		//delay of spawning a meteor

	float		m_meteor2Spd;		//speed of meteor 2
	float		m_meteor2Radius;	//the radius of the meteor 2
	win32Time	m_meteor2Delay;		//delay of spawning a meteor 2
	int			m_meteor2Max;		//max number of meteors
	int			m_meteor2Cur;		//current number of meteors made

	//////////////////////////////////////////
	// Fire Force Field
	hOBJ		m_ffieldOBJ;		//the force field object
	Move		m_ffieldRotSpd;		//force field rotation speed
	hTXT		m_ffieldTxtFX;		//force field explosion FX texture
	bool		m_bFfieldOn;		//is force field on? (only set by FFieldActivate)

	//////////////////////////////////////////
	// Gaseous Area
	Id			m_gasID;			//gas attack

	hTXT		m_gasTxt;			//gas FX texture
	float		m_gasSize;			//gas particle size

	D3DXVECTOR3	m_gasBound;			//the gas bound

	//////////////////////////////////////////
	// Teleport
	float				  m_teleEquatorY;	//determines where to teleport
	vector< D3DXVECTOR3 > m_teleTop;		//teleportation locations at top
	vector< D3DXVECTOR3 > m_teleBottom;		//teleportation locations at bottom

	win32Time			  m_teleActivateDelay;	//delay of activation
	win32Time			  m_teleGrowDelay;		//delay of teleport growth (after activation)
	float				  m_teleScale;			//the current scale (< 1 means we need it to grow)

	int					  m_teleInd;			//the index used for teleportation

	//////////////////////////////////////////
	// Ta-Ta Mahatta
	float				  m_mahattaScale;		//maximum scale
	win32Time			  m_mahattaDelay;		//the scale delay
};

#endif