#ifndef _tata_projectiles_common_h
#define _tata_projectiles_common_h

#include "tata_projectiles.h"

#include "tata_sound.h"

//
// This file includes all stuff about level objects (levers, platforms...etc)
//

/////////////////////////////////////////////////////////////////////////////
// Push Melee (for Ta-Ta Tug)
/////////////////////////////////////////////////////////////////////////////
class PushMelee : public Projectile {
public:
	PushMelee(const Id & owner, float pushSpd);
	~PushMelee();

	//get the push velocity
	D3DXVECTOR3 GetPushVel();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	Move	m_pushSpd;		//speed used for pushing.
};

/////////////////////////////////////////////////////////////////////////////
// Use Melee (for Tinker Ta)
/////////////////////////////////////////////////////////////////////////////
class UseMelee : public Projectile {
public:
	UseMelee(const Id & owner, const D3DXVECTOR3 & bound, int jointBegin, int jointEnd);
	~UseMelee();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	int m_jBInd, m_jEInd;	//joint 'B'egin and 'E'nd index
};

/////////////////////////////////////////////////////////////////////////////
// Open Melee (for Key Key Ta)
/////////////////////////////////////////////////////////////////////////////
class OpenMelee : public Projectile {
public:
	OpenMelee(const Id & owner);
	~OpenMelee();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
};

/////////////////////////////////////////////////////////////////////////////
// Pull Power (for Chi-Ta)
/////////////////////////////////////////////////////////////////////////////
class PullPower : public Projectile {
public:
	PullPower(const Id & owner, const D3DXVECTOR3 & bound, float length, float spd);
	~PullPower();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	float		m_maxLength;
	float		m_curLength;
	Move		m_spd;
};

/////////////////////////////////////////////////////////////////////////////
// Attack Melee (for Ta-Ta Trample)
// The given 'bound' must have positive values for x,y,z
/////////////////////////////////////////////////////////////////////////////
class AttackMelee : public Projectile {
public:
	AttackMelee(const Id & owner, const D3DXVECTOR3 & bound, int jointInd);
	~AttackMelee();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	int			m_jointInd;		//the joint where the attack is located
};

/////////////////////////////////////////////////////////////////////////////
// Attack Melee 2 (similar to AttackMelee, uses two end points rather than one)
// The given 'bound' must have positive values for x,y,z
/////////////////////////////////////////////////////////////////////////////
class AttackMelee2 : public Projectile {
public:
	AttackMelee2(const Id & owner, const D3DXVECTOR3 & bound, int jointBegin, int jointEnd);
	~AttackMelee2();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	int m_jBInd, m_jEInd;	//joint 'B'egin and 'E'nd index
};

/////////////////////////////////////////////////////////////////////////////
// SPIT!!! (for Shroom Shooter)
/////////////////////////////////////////////////////////////////////////////

//DON'T USE THIS!!!!
class _Spit : public Projectile {
public:
	_Spit(const Id & owner, float spd, bool bBounce = false, int bounceMax = 1, int sndInd = -1);
	~_Spit();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	Move	m_spd;		//speed of projectile
	bool	m_bBounce;

	int		m_bounceCur;
	int		m_bounceMax;

	HCHANNEL m_tSnd;		//travelling sound
};

class Spit : public _Spit {
public:
	Spit(const Id & owner, D3DXVECTOR3 loc, float spd, hMDL spitMdl, hTXT trailTxt,
		BYTE r, BYTE g, BYTE b, bool bBounce = false, int bounceMax = 1, int sndInd = -1);

	~Spit();

private:
	BYTE	m_r,m_g,m_b; //particle color

	hTXT	m_trailTxt;	//the texture trail thing
};

class Spit2 : public _Spit {
public:
	Spit2(const Id & owner, D3DXVECTOR3 loc, float spd, hMDL spitMdl, hTXT EnergyTxt,
		float radius, BYTE r, BYTE g, BYTE b, bool bBounce = false, int bounceMax = 1,
		int sndInd = -1);

	~Spit2();

private:
	hPARFX	m_energyFX;	//big ball of energy
};

/////////////////////////////////////////////////////////////////////////////
// Scanner (for Funga Spy)
/////////////////////////////////////////////////////////////////////////////
class Scanner : public Projectile {
public:
	Scanner(const Id & owner, int jointBeginInd, int jointEndInd, float length);

	~Scanner();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int m_jBInd, m_jEInd;		//joint indices

	D3DXVECTOR3 m_normJoint;	//normalized joint dir (model space)

	float		m_length;

	float		m_tLast;
};

/////////////////////////////////////////////////////////////////////////////
// Whip (for Loop-Ta-Loop)
/////////////////////////////////////////////////////////////////////////////
class Whip : public Projectile {
public:
	Whip(const Id & owner, int whipEndInd, float lengthMax, float spd, double stunDelay, int grappleState);

	~Whip();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int	m_grappleState;			//state to change the object when grappling on a steak
	int m_whipEndInd;			//whip end joint index

	D3DXVECTOR3 m_normJoint;	//normalized joint dir (model space)

	float		m_lengthAccum;
	float		m_lengthMax;

	double		m_stunDelay;	//stun delay

	Move		m_spd;
};

/////////////////////////////////////////////////////////////////////////////
// Frost (for Frost-Ta)
/////////////////////////////////////////////////////////////////////////////
class Frost : public Projectile {
public:
	Frost(const Id & owner, const D3DXVECTOR3 & bound, int jBInd, int jEInd, 
		hTXT frostFXTxt, hMDL frozenMdl, hTXT frozenTxt, BYTE r, BYTE g, BYTE b);

	~Frost();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int		m_jBInd, m_jEInd;	//joint index begin and end
	hTXT	m_frostFXTxt;		//particle texture
	BYTE	m_r,m_g,m_b;		//particle color

	hMDL	m_frozenMdl;		//the frozen model for block chunks
	hTXT	m_frozenTxt;		//the texture used to make a creature look frozen

	hPARFX	m_particle;			//the particle handle
};

/////////////////////////////////////////////////////////////////////////////
// Gasus (gas projectile)
// use duration = -1 for infinite
/////////////////////////////////////////////////////////////////////////////
class Gasus : public Projectile {
public:
	Gasus(const Id & owner, const D3DXVECTOR3 & loc,
		const D3DXVECTOR3 & bound, const D3DXVECTOR3 & gasDir, 
		hTXT gasTxt, float gasSize, BYTE r, BYTE g, BYTE b, float duration);

	~Gasus();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	win32Time	m_duration;			//duration of gas
	bool		m_bInfinite;		//no duration?
	hPARFX		m_particle;			//the particle handle
};

/////////////////////////////////////////////////////////////////////////////
// Teleport (Teleportation!)
/////////////////////////////////////////////////////////////////////////////
class TeleportProj : public Projectile {
public:
	TeleportProj(const Id & owner, const D3DXVECTOR3 & loc);
	~TeleportProj();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	int m_state;		//0 = init., 1 = collided, 2 = OK!
	D3DXVECTOR3 m_dest;	//destination
};

/////////////////////////////////////////////////////////////////////////////
// Spike
/////////////////////////////////////////////////////////////////////////////
class Spike : public Projectile {
public:
	Spike(const Id & owner, hMDL model, const D3DXVECTOR3 & loc, 
		float endY, float spd);
	~Spike();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	Move		m_spikeSpd;		 //speed for spike
	float		m_spikeMaxHeight;//maximum height of spike before going down
	float		m_spikeMinHeight;//maximum height of spike before disappearing
	float		m_spikeDir;		 //going up or down
};

#endif