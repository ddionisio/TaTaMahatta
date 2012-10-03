#ifndef _tata_object_common_h
#define _tata_object_common_h

#include "tata_object.h"

#include "tata_sound.h"

//
// This file includes all stuff about level objects (levers, platforms...etc)
//

/////////////////////////////////////////////////////////////////////////////
// Block
/////////////////////////////////////////////////////////////////////////////
class Block : public Object {
public:
	Block();
	~Block();

	const char *GetSubTypeStr() const { return "block"; }

	//set the debris model for this block
	void SetDebrisModel(hMDL mdl);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	hMDL	m_debrisMdl;		//the model used when this block is destroyed
};

class Platform : public Object, public WaypointDat {
public:
	Platform();
	~Platform();

	void SetMoveSpd(float f) { m_moveSpd.MoveSetSpeed(f); }	//set movement speed
	float GetMoveSpd() { return m_moveSpd.MoveGetSpeed(); } //get movement speed

	const char *GetSubTypeStr() const { return "platform"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	Move		m_moveSpd;			//movement speed
};

/////////////////////////////////////////////////////////////////////////////
// Button
/////////////////////////////////////////////////////////////////////////////
class Button : public Object {
public:
	Button();
	~Button();

	const char *GetSubTypeStr() const { return "button"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	// if section is NULL, then
	// the entire file is parsed.
	// format of section: [<section>_begin]
	// 				      ... // code
	//					  [<section>_end]
	//bOn == true  if the script is for 'on'
	//bOn == false if the script is for 'off'
	RETCODE LoadScript(bool bOn, const char *filename, const char *section);

	//destroys current script
	//bOn == true  if the script is for 'on'
	//bOn == false if the script is for 'off'
	void    RemoveScript(bool bOn);

	void    SetOnLoc(const D3DXVECTOR3 & loc) { m_onLoc = loc; }
	void    SetOffLoc(const D3DXVECTOR3 & loc) { m_offLoc = loc; }

private:
	hSCRIPT		m_onScript;		//the script to execute when turned 'on'
	hSCRIPT		m_offScript;	//the script to execute when turned 'off'

	D3DXVECTOR3 m_onLoc;		//the location when 'on'
	D3DXVECTOR3 m_offLoc;		//the location when 'off'

	Move		m_moveSpd;		//the spd for moving

	bool		m_bMoving;		//are we moving?
	bool		m_bLastOn;		//last 'on' state
	bool		m_bOnce;		//set to 'true' if we only go once.
	bool		m_bProjPush;	//pushed by a projectile
};

/////////////////////////////////////////////////////////////////////////////
// Lever
/////////////////////////////////////////////////////////////////////////////
class Lever : public Object {
public:
	Lever();
	~Lever();

	const char *GetSubTypeStr() const { return "lever"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

	// if section is NULL, then
	// the entire file is parsed.
	// format of section: [<section>_begin]
	// 				      ... // code
	//					  [<section>_end]
	//bOn == true  if the script is for 'on'
	//bOn == false if the script is for 'off'
	RETCODE LoadScript(bool bOn, const char *filename, const char *section);

	//destroys current script
	//bOn == true  if the script is for 'on'
	//bOn == false if the script is for 'off'
	void    RemoveScript(bool bOn);

	void    SetOnLoc(const D3DXVECTOR3 & loc) { m_onLoc = loc; }
	void    SetOffLoc(const D3DXVECTOR3 & loc) { m_offLoc = loc; }

private:
	hSCRIPT		m_onScript;		//the script to execute when turned 'on'
	hSCRIPT		m_offScript;	//the script to execute when turned 'off'

	D3DXVECTOR3 m_onLoc;		//the location when 'on'
	D3DXVECTOR3 m_offLoc;		//the location when 'off'

	bool		m_bMoving;		//are we moving?
	bool		m_bLastOn;		//last 'on' state
	bool		m_bOnce;		//set to 'true' if we only go once.
};

/////////////////////////////////////////////////////////////////////////////
// Doodad
/////////////////////////////////////////////////////////////////////////////
class Doodad : public Object, public WaypointDat {
public:
	Doodad();
	~Doodad();

	const char *GetSubTypeStr() const { return "doodad"; }

	void SetMoveSpd(float f) { m_moveSpd.MoveSetSpeed(f); }	//set movement speed
	float GetMoveSpd() { return m_moveSpd.MoveGetSpeed(); } //get movement speed

	const char *GetCollectionReq();	//collection requirement string
	void SetCollectReq(const char *str);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

protected:

private:
	string		m_collectReq;
	Move		m_moveSpd;			//movement speed
};

/////////////////////////////////////////////////////////////////////////////
// Sign
/////////////////////////////////////////////////////////////////////////////
class Sign : public Object {
public:
	Sign();
	~Sign();

	void SetImageDisplay(const Id & ID);

	void Show(bool bShow); //show sign?

	const char *GetSubTypeStr() const { return "sign"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	bool	m_bCollide;		//collided

	Id		m_imageID;		//image display ID in the world
};

/////////////////////////////////////////////////////////////////////////////
// Trigger
/////////////////////////////////////////////////////////////////////////////
class Trigger : public Object {
public:
	Trigger();
	~Trigger();

	// if section is NULL, then
	// the entire file is parsed.
	// format of section: [<section>_begin]
	// 				      ... // code
	//					  [<section>_end]
	RETCODE LoadScript(const char *filename, const char *section);

	//destroys current script
	void    RemoveScript();

	Id GetCollideWith();

	const char *GetSubTypeStr() const { return "trigger"; }

	void AllowMultipleEntities(bool bYes);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	bool	m_bCollide;	//collided

	hSCRIPT	m_onScript;	//the script to execute when turned 'on'

	bool	m_bOnce;	//set to 'true' if we only go once.

	bool	m_allowMultiple;	//allow multiple entities to activate the trigger

	Id		m_collideWith;	//the Entity inside the trigger
};

/////////////////////////////////////////////////////////////////////////////
// Goal (used for gathering all Ta-Tas to spot, rescuing...etc.)
/////////////////////////////////////////////////////////////////////////////
class Goal : public Object {
public:
	enum {
		GOAL_TATAHOME,	//get Ta-Ta home, Ta-Ta will be removed once home
		GOAL_RESCUE,	//rescue little Ta-Tas
		GOAL_GATHER,	//gather something based on ID
		GOAL_DESTROY,	//if the ID does not exist, then goal is incremented
		GOAL_MAX
	};

	Goal();
	~Goal();

	int  GetGoalType(const char *typeName);
	void SetGoal(int type);

	void SetRequiredID(const char *IDName);

	Id GetCollideWith();

	const char *GetSubTypeStr() const { return "goal"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int		m_goalType;		//type of goal

	bool	m_bCollide;

	string  m_collideID;	//required collision ID (entity name), "none" for any

	Id		m_collideWith;	//the Entity inside the goal
};

/////////////////////////////////////////////////////////////////////////////
// Turret
/////////////////////////////////////////////////////////////////////////////
class Turret : public Object {
public:
	Turret();
	~Turret();

	void SetProjMDL(hMDL mdl);
	void SetProjTXT(hTXT txt);
	void SetProjSpd(float spd);
	void SetProjDelay(double d);
	void SetProjClr(BYTE r, BYTE g, BYTE b);

	const char *GetSubTypeStr() const { return "turret"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	hMDL		m_projMdl;			//the model of the projectile
	hTXT		m_projFXTxt;		//the projectile particle FX texture

	BYTE		m_r,m_g,m_b;
	
	float		m_attackSpd;		//the speed of the projectile
	win32Time	m_attackDelay;		//delay before spitting again

	win32Time	m_soundDelay;		//don't play too many...
};

/////////////////////////////////////////////////////////////////////////////
// Steak
/////////////////////////////////////////////////////////////////////////////
class Steak : public Object {
public:
	Steak();
	~Steak();

	const char *GetSubTypeStr() const { return "steak"; }

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
};

/////////////////////////////////////////////////////////////////////////////
// Ouch Field
/////////////////////////////////////////////////////////////////////////////
class OuchField : public Object {
public:
	OuchField();
	~OuchField();

	const char *GetSubTypeStr() const { return "ouchfield"; }

	void OuchSetHitAmt(int hit);
	void OuchSetGasFlag(bool bYes);
	void OuchSetFX(hTXT gasImg, BYTE r, BYTE g, BYTE b);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int		m_hitAmt;		//amt of hit
	bool	m_bGas;			//is this field a gas area?
};

/////////////////////////////////////////////////////////////////////////////
// Geyser
/////////////////////////////////////////////////////////////////////////////
class Geyser : public Object {
public:
	Geyser();
	~Geyser();

	void GeyserSetAlwaysActive(bool bYes);
	void GeyserSetActivateDelay(double delay);
	void GeyserSetInActivateDelay(double delay);

	void GeySerSetUpSpd(float amt);

	void GeyserInitParticle(hTXT gasImg, BYTE r, BYTE g, BYTE b); //make sure you set the upSpd first

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	Move		m_upSpd;			//movement speed
	hPARFX		m_particle;			//the particle used by this object
	win32Time	m_activeDelay;		//delay while active
	win32Time	m_deactiveDelay;	//delay while inactive
	bool		m_bAlwaysActive;	//are we always active?

	bool		m_bInActive;		//currently inactive?

	list<Id>	m_entities;			//the entities that hit us...
};

/////////////////////////////////////////////////////////////////////////////
// Teleport
/////////////////////////////////////////////////////////////////////////////
class Teleport : public Object {
public:
	Teleport();
	~Teleport();

	void TeleportSetDest(const D3DXVECTOR3 & loc); //set the destination
	void TeleportSetFX(hTXT txt, BYTE r, BYTE g, BYTE b);
	
	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	D3DXVECTOR3	m_dest;
};

/////////////////////////////////////////////////////////////////////////////
// Lightning Area
/////////////////////////////////////////////////////////////////////////////
class LightningArea : public Object {
public:
	LightningArea();
	~LightningArea();

	void LightningInitFX(fxLightningY_init *pDat);

	void LightningSetDeactivateDelay(double d);

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	hPARFX		m_lightning;

	win32Time	m_deactivateDelay;	//delay of deactivation, once activated

	bool		m_bInfinite;		//is it infinite?
};

#endif