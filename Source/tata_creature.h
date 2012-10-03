#ifndef _tata_creature_h
#define _tata_creature_h

#include "tata_ID.h"
#include "tata_basic.h"
#include "tata_bound.h"
#include "tata_waypoint.h"

#include "tata_entityparse.h"

#include "tata_collision.h"

#include "tata_creature_status.h"

//this is the Abstract Base Class for Enemies and Ta-Tas

class Creature : public EntityCommon, public WaypointDat
{
public:
	Creature(eEntityType type, int subType);
	virtual ~Creature();

	void SetCurHit(int n) { m_curHits = n; } //set current hitpoints
	void SetMaxHit(int n) { m_maxHits = n; } //set max hitpoints
	float GetCurHit() { return m_curHits; }  //get current hitpoints
	float GetMaxHit() { return m_maxHits; }  //get max hitpoints

	void SetMoveSpd(float f) { m_moveSpd.MoveSetSpeed(f); }	//set movement speed
	void SetRotSpd(float f) { m_rotSpd.MoveSetSpeed(f); }   //set rotation speed
	float GetMoveSpd() { return m_moveSpd.MoveGetSpeed(); } //get movement speed
	float GetRotSpd() { return m_rotSpd.MoveGetSpeed(); }   //get rotation speed

	Move GetMove() { return m_moveSpd; } //get the Move class for walking
	Move GetRotMove() { return m_rotSpd; } //get the Move class for rotating

	float GetMoveSpdUpdate(); //grabs the actual move in terms of time

	//move creature to current direction, s is usu. -1 or 1. 
	//Updates velocity
	void MoveDir(float s);

	//move creature with given vector, s is usu. -1 or 1. 
	//Updates velocity
	void MoveVec(const D3DXVECTOR3 & v, float s);

	//strafe creature, s is usu. -1 or 1. 
	//Updates velocity, left < 0, right > 0
	void MoveStrafe(float s);

	//rotate creature's current direction, s is usu. -1 or 1. 
	//Updates direction
	//returns the rotation amt. in terms of time
	float Rotate(float s);

	//input functions
	virtual void InputRotate();	//rotate by INP_LEFT or INP_RIGHT
	virtual void InputStrafe();	//strafe by holding INP_E and INP_LEFT or INP_RIGHT
	virtual void InputWalk();	//move forward by INP_UP or INP_DOWN

	// Load creature
	RETCODE Load(const char *dataTypeName);

	//used for statue stuff
	static hMDL LoadModel(hCFG datFile);
	static hOBJ LoadObject(hCFG datFile, hMDL mdl);
	static hTXT LoadPortrait(hCFG datFile);

	// if section is NULL, then
	// the entire file is parsed.
	// format of section: [<section>_begin]
	// 				      ... // code
	//					  [<section>_end]
	RETCODE LoadScript(const char *filename, const char *section);

	//destroys current script
	void    RemoveScript();

	RETCODE Update();		//update

	// Teleportation is a third way process:
	// first you call this, it creates a teleport projectile
	// second, the projectile creates the cre. status 'teleport'
	// then checks to see if destination is clear, if so...
	// third, the status does some snazzy FX and
	// teleports creature
	void Teleport(const char *targetName); //teleport this sucker to a new target
	void Teleport(const D3DXVECTOR3 & loc); //teleport this sucker to a new location

	bool Hit(int amt=1);		//get hit, return true if we deduced hitpoints
								//this creature will die if health point reaches 0

	void Stun(double delay);	//get stunned
	
	void Freeze(hTXT frozenTxt, hMDL frozenChunk);	//this will freeze the creature and turn into a block
													//the creature will be killed

	//returns the object animation state for death
	virtual int GetDeathState() { return 0; }

	//returns the object animation state for ouch
	virtual int GetOuchState() { return 0; }

	//returns the creature status
	eCreStatus GetStatus();

	//get the pointer to creature status (DO NOT DELETE)
	CreatureStatus *GetCreStatus();

	//set creature status (DO NOT DELETE THE GIVEN POINTER AFTER CALLING THIS)
	void SetCreStatus(CreatureStatus *pCreStatus);

	//play sound within the creature
	//ind is index in the sound list
	HCHANNEL CREPlaySound(int ind);

private:
	int			m_curHits;			//the current number of hitpoints
	int			m_maxHits;			//the max number of hitpoints
	hSCRIPT		m_script;			//the script, updated if exists
	
	Move		m_moveSpd;			//movement speed
	Move		m_rotSpd;			//rotation speed

	CreatureStatus *m_pCreStatus;	//if not NULL, then this creature is not normal...
};

#endif