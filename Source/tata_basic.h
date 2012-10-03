#ifndef _tata_basic_h
#define _tata_basic_h

#include "tata_ID.h"
#include "tata_bound.h"

//basic classes inherited by various other complex classes...

//Nameable...
class Name {
public:
	Name() {}
	Name(const char *str) : m_name(str) {}
	~Name() {}

	const char *GetName() { return m_name.c_str(); }
	void SetName(const char *name) { m_name = name; }
protected:
	string m_name;					//Unique name
};

//Flag
class Flag {
public:
	Flag() : m_flags(0) {}

	FLAGS		GetFlags() { return m_flags; }

	void SetFlag(int mask, bool bSet)
	{
		if(bSet)
			SETFLAG(m_flags, mask);
		else
			CLEARFLAG(m_flags, mask);
	}

	bool		CheckFlag(int mask) { return TESTFLAGS(m_flags, mask); }

protected:
	FLAGS m_flags;		//the flags
};

//Movement...for moving.
class Move {
public:
	Move() : m_spd(0) {}
	~Move() {}

	void MoveSetSpeed(float spd) { m_spd = spd; }
	float MoveGetSpeed()		 { return m_spd; }

	float MoveUpdate(double timeFactor);

protected:
	float		m_spd;				//spd amount
};

//All creatures, level objects, projectiles and items must inherit this!!!!
class EntityCommon : public GameID, public Bound, public Name, public Flag {
public:
	EntityCommon(void *thisPtr, eEntityType type, int subType);
	virtual ~EntityCommon();

	//NOTE: set this to pure virtual
	virtual RETCODE Update(){ return RETCODE_SUCCESS; }

	//NOTE: set this to pure virtual
	virtual int Callback(unsigned int msg, unsigned int wParam, int lParam) { return 1; }

	//Animate based on collision reaction
	virtual void MoveAnim(eMoveType type) {}

	//collide to entity
	virtual void CollideEntity(EntityCommon *pEntity, const gfxTrace & trace) { }

	//NOTE: set this to pure virtual
	virtual void CollideWorld(hQBSP qbsp, const gfxTrace & trace) { }

	//ENTITY_TYPE_TATA, ENTITY_TYPE_ENEMY,
	//ENTITY_TYPE_OBJECT, ENTITY_TYPE_PROJECTILE,
	//ENTITY_TYPE_ITEM
	eEntityType GetEntityType() const { return m_type; }

	//return the string version of entity type
	const char * GetEntityTypeStr() const;

	int GetSubType() const { return m_subType; } //get sub type

	//return the string version of entity sub-type
	virtual const char *GetSubTypeStr() const { return "dumbass"; }

	D3DXVECTOR3 GetLoc() { return m_loc; }
	D3DXVECTOR3 GetDir() { return m_dir; }
	D3DXVECTOR3 GetVel() { return m_vel; }

	void		SetOBJ(hOBJ obj); //ONCE SET, DON'T DELETE THE 'obj' THAT YOU PASSED!
	hOBJ		GetOBJ() { return m_obj; } //note: do not destroy object!

	void Show(bool bShow);	//disable/enable display

	void SetLoc(const D3DXVECTOR3 & v);
	void SetDir(const D3DXVECTOR3 & v);
	
	//returns true if velocity has been set
	bool SetVel(const D3DXVECTOR3 & v);

	//add an entity to push
	void AddPushList(const Id & ID);

	//grabs the object who collided with us from the previous collision call.
	Id GetLastCollided() { return m_LastCollided; }

	gfxTrace GetLastTrace() { return m_gfxTrace; }

	void CollAddEntity(const Id & ID);
	void CollClear();

	//do something based on player profile
	//true = activate, false  = deactivate
	//depending on type
	void ProfileSetReqCollection(const char *str);
	void ProfileSetReqStageLevel(int stage, int level);

	//this will do stuff depending on given profile
	void ProfileCheckReq(hCFG profile);

	void SetSpawnLoc(const D3DXVECTOR3 & loc) { m_spawnLoc = loc; }
	D3DXVECTOR3 GetSpawnLoc() { return m_spawnLoc; }

	//Entity has lots of friends...after that one night from a party...
	friend inline void _CollisionEntities(hQBSP qbsp, EntityCommon *pEntity);
	friend void _CollisionMap(hQBSP qbsp, EntityCommon *pEntity);
	friend void _CollisionMapShadow(hQBSP qbsp, EntityCommon *pEntity);
	friend void _CollisionEntitiesUpdate(list< Id > & theList, bool bSetGoal);
	friend void __fastcall _CollidePair(hQBSP qbsp, EntityCommon *pEntity1, EntityCommon *pEntity2);
	friend void _BrutalCollision(hQBSP qbsp, list< Id > & group);
	friend void CollisionEntitiesDoVelocity();
	friend void _CollisionMap(hQBSP qbsp, EntityCommon *pEntity);
	friend void _CollisionSet(hQBSP qbsp, EntityCommon *pEntity);
	friend void CollisionExecute(hQBSP qbsp);
	friend void CollisionBltShadows();

protected:
	eEntityType	m_type;

	int			m_subType;			//the sub type of this entity. eg: OBJECT_BLOCK, TATA_CAPTAINTATA...etc

	hOBJ		m_obj;

	D3DXVECTOR3 m_spawnLoc;			//optional spawn location (used for some creatures)

private:
	D3DXVECTOR3	m_loc;				//the location
	D3DXVECTOR3	m_dir;				//the direction

	D3DXVECTOR3 m_vel;				//the accumulated velocity to move

	//used by profile stuff
	string		m_saveCollectReq;			//the required collection from profile ("none" for not required)
	int			m_saveStage, m_saveLevel;	//the stage and level that needs to be unlocked (-1 for not required)

	//used by collision
	float		m_tShadow;			//the t val. used for shadow
	D3DXVECTOR3	m_shadowLoc;		//the location used for shadow

	bool		m_bPushed;			//are we getting pushed?
	Id			m_LastCollided;		//last entity to collide to
	bool		m_bcollideWith;		//who did we collide with? (invalid if world)
	gfxTrace	m_gfxTrace;			//used by collision.

	list<Id>	m_collideList;		//list of potential entity collision

	list<Id>	m_PushList;			//list of entities to push.

	//get pushed by the entities from the push list
	void ExecutePushList();
};

#endif