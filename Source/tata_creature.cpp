#include "tata_main.h"

#include "tata_creature.h"
#include "tata_creature_tata.h"
#include "tata_creature_enemy.h"

#include "tata_globals.h"

#include "tata_object_common.h"

#include "tata_projectiles_common.h"

#include "tata_sound.h"

//all base creature stuff

Creature::Creature(eEntityType type, int subType) : EntityCommon(this, type, subType), 
m_curHits(0), m_maxHits(0), m_script(0), m_pCreStatus(0)
{
	D3DXVECTOR3 dir(0,0,-1);

	SetDir(dir);

	SetFlag(ENTITY_FLAG_PULLABLE, true);

	//all creatures cast shadow
	SetFlag(ENTITY_FLAG_SHADOW, true);
}

Creature::~Creature()
{
	if(m_script)
	{ ScriptDestroy(this, &m_script); m_script = 0; }

	if(m_pCreStatus)
		delete m_pCreStatus;
}

//used for statue stuff
hMDL Creature::LoadModel(hCFG datFile)
{
	hMDL model=0;

	char filename[MAXCHARBUFF];

	//////////////////////////////////////
	//load model
	if(CfgGetItemStr(datFile, "main", "model", filename))
	{
		string mdlFile = GAMEFOLDER;
		mdlFile += "\\";
		mdlFile += filename;

		model = MDLCreate(0, mdlFile.c_str());

		if(!model)
		{ ErrorMsg("Creature::LoadModel", "Unable to load model", true); return 0; }
	}

	return model;
}

hOBJ Creature::LoadObject(hCFG datFile, hMDL mdl)
{
	hOBJ obj=0;

	char filename[MAXCHARBUFF];

	//////////////////////////////////////
	//load animation and create object
	if(CfgGetItemStr(datFile, "main", "anifile", filename))
	{
		string aniFile = GAMEFOLDER;
		aniFile += "\\";
		aniFile += filename;

		obj = OBJCreate(0, mdl, 0, 0, 0, eOBJ_ANIMATE, aniFile.c_str());

		if(!obj)
		{ ErrorMsg("Creature::LoadObject", "Unable to create object", true); return 0; }
	}

	return obj;
}

hTXT Creature::LoadPortrait(hCFG datFile)
{
	char filename[MAXCHARBUFF];

	hTXT txt = 0;
	
	if(CfgGetItemStr(datFile, "main", "portrait", filename))
	{
		string portFile = GAMEFOLDER;
		portFile += "\\";
		portFile += filename;

		txt = TextureCreate(0, portFile.c_str(), false, 0);

		if(!txt)
		{ ErrorMsg("Creature::LoadPortrait", "Unable to load portrait", true); return 0; }
	}

	return txt;
}

// Load creature
//if wayptID not NULLL, then waypoint node is created
//if scriptFile not NULL, then script is loaded with script[Begin,End]
RETCODE Creature::Load(const char *dataTypeName)
{
	//m_name = dataTypeName;

	const char *datafilename = TataDataGetFilePath(dataTypeName);

	hCFG cfg=0;
	
	if(datafilename)
		cfg = CfgFileLoad(datafilename);

	if(!cfg)
	{ ErrorMsg("Creature::Load", "Data File Not Found", false); return RETCODE_FAILURE; }

	//load the basic stuff
	hMDL model;

	//////////////////////////////////////
	//load hitpoints
	int hp = CfgGetItemInt(cfg, "main", "maxhp"); if(hp < 0) { hp = 1; }

	SetCurHit(hp);
	SetMaxHit(hp);

	//////////////////////////////////////
	//load model
	model = LoadModel(cfg);

	//////////////////////////////////////
	//load animation and create object
	m_obj = LoadObject(cfg, model);

	//////////////////////////////////////
	//load portrait, ONLY IF CREATURE IS A TATA!!!
	if(GetEntityType() == ENTITY_TYPE_TATA)
	{
		TaTaCommon *pTata = (TaTaCommon *)this;
		pTata->SetPortrait(LoadPortrait(cfg));
	}

	//////////////////////////////////////
	//load speed
	float val;

	val = CfgGetItemFloat(cfg, "main", "movespd");

	if(val > 0)
		m_moveSpd.MoveSetSpeed(val);

	val = CfgGetItemFloat(cfg, "main", "rotspd");

	if(val > 0)
		m_rotSpd.MoveSetSpeed(val);

	//////////////////////////////////////
	//load flag
	SetFlag(CfgGetItemInt(cfg, "main", "flag"), true);

	//////////////////////////////////////
	//load bounds
	D3DXVECTOR3 bMin, bMax;

	bMin.x = CfgGetItemFloat(cfg, "bound", "minx");
	bMin.y = CfgGetItemFloat(cfg, "bound", "miny");
	bMin.z = CfgGetItemFloat(cfg, "bound", "minz");

	bMax.x = CfgGetItemFloat(cfg, "bound", "maxx");
	bMax.y = CfgGetItemFloat(cfg, "bound", "maxy");
	bMax.z = CfgGetItemFloat(cfg, "bound", "maxz");

	BoundSetMin(bMin);
	BoundSetMax(bMax);

	BoundSetRadius(CfgGetItemFloat(cfg, "bound", "radius"));

	/////////////////////////////////
	//generate planes
	BoundGeneratePlanes();

	//////////////////////////////////////
	//call the callback for special loading
	RETCODE ret = Callback(ENTITYMSG_CFGLOAD, (WPARAM)cfg, 0);

	CfgFileDestroy(&cfg);

	return ret;
}

// if section is NULL, then
// the entire file is parsed.
// format of section: [<section>_begin]
// 				      ... // code
//					  [<section>_end]
RETCODE Creature::LoadScript(const char *filename, const char *section)
{
	RemoveScript();

	char beginStr[MAXCHARBUFF], endStr[MAXCHARBUFF];

	strcpy(beginStr, "[");
	strcat(beginStr, section);
	strcat(beginStr, "_begin]");

	strcpy(endStr, "[");
	strcat(endStr, section);
	strcat(endStr, "_end]");

	Id CreID = GetID();

	m_script = ScriptLoad(&CreID, filename, beginStr, endStr);

	return RETCODE_SUCCESS;
}

//destroys current script
void Creature::RemoveScript()
{
	if(m_script)
	{ 
		Id CreID = GetID();

		ScriptDestroy(&CreID, &m_script);

		m_script = 0;
	}
}

void Creature::MoveDir(float s)
{
	SetVel(GetVel() + ((s*m_moveSpd.MoveUpdate(g_timeElapse))*GetDir()));
}

void Creature::MoveVec(const D3DXVECTOR3 & v, float s)
{
	SetVel(GetVel() + ((s*m_moveSpd.MoveUpdate(g_timeElapse))*v));
}

void Creature::MoveStrafe(float s)
{
	//get the strafe vector
	D3DXVECTOR3 uVec(0,1,0);
	D3DXVECTOR3 sVec;
	D3DXVec3Cross(&sVec, &GetDir(), &uVec);
	D3DXVec3Normalize(&sVec, &sVec);

	MoveVec(sVec, s);
}

float Creature::GetMoveSpdUpdate()
{
	return m_moveSpd.MoveUpdate(g_timeElapse);
}

float Creature::Rotate(float s)
{
	float spd = m_rotSpd.MoveUpdate(g_timeElapse);
	D3DXVECTOR3 dir = GetDir();

	D3DXMATRIX theMtx;

	//reduce rotation speed when moving forward/backward
	//if you are a Ta-Ta
	if((InputIsDown(INP_UP) || InputIsDown(INP_DOWN)) 
		&& GetEntityType() == ENTITY_TYPE_TATA)
		D3DXMatrixRotationY(&theMtx, s*spd*0.5f);
	else
		D3DXMatrixRotationY(&theMtx, s*spd);

	D3DXVec3TransformNormal(&dir, &dir, &theMtx);

	SetDir(dir);
	
	return spd;
}

//input functions
void Creature::InputRotate()
{
	if(InputIsDown(INP_LEFT))
		Rotate(1);
	else if(InputIsDown(INP_RIGHT))
		Rotate(-1);
}

void Creature::InputStrafe()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK)
		|| (tataOBJState == CREANIM_FALL))
	{
		if(InputIsDown(INP_LEFT))
			MoveStrafe(-0.5f);
		else if(InputIsDown(INP_RIGHT))
			MoveStrafe(0.5f);
	}
}

void Creature::InputWalk()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK))
	{
		//check to see if the 'up'/'down' button is down
		//if so, move!
		if(InputIsDown(INP_UP))
		{
			if(CheckFlag(TATA_FLAG_RUN))
				MoveDir(g_runScale);
			else
				MoveDir(1);
		}
		else if(InputIsDown(INP_DOWN))
		{
			MoveDir(-1);
			SetFlag(TATA_FLAG_BACKWALK, true);
		}
	}
}

RETCODE Creature::Update()
{
	//update the basic stuff

	////////////////////////////////////////////////
	// Update the status
	if(m_pCreStatus)
	{
		RETCODE statusRet = m_pCreStatus->Update();

		if(statusRet == RETCODE_STATUS_DONE)
		{ delete m_pCreStatus; m_pCreStatus = 0; }
		else if(statusRet == RETCODE_STATUS_NOUPDATE)
			return RETCODE_SUCCESS;
	}

	////////////////////////////////////////////////
	// Update waypoints, if available
	if(m_pWayPt)
	{
		D3DXVECTOR3 destPt;

		m_pWayPt->GetCurrentLinearLoc(&destPt);

		D3DXVECTOR3 dir(destPt - GetLoc());
		dir.y = 0;

		float len = D3DXVec3Length(&dir);

		float spd = m_moveSpd.MoveUpdate(g_timeElapse);
		
		if(len > spd)
		{
			dir /= len;

			SetDir(dir);

			SetVel(GetVel() + (spd*dir));
		}
		//we are close enough
		//get to next waypoint node
		else
		{
			SetVel(GetVel() + dir);

			m_pWayPt->GotoNext();

			SetFlag(CRE_FLAG_TAG, false);
		}
		
		if(m_pWayPt->IsDone())
			RemoveWayPt();
	}

	//call the callback for special update
	return Callback(ENTITYMSG_UPDATE,0,0);
}

//teleport this sucker to a new target
void Creature::Teleport(const char *targetName)
{
	if(m_pCreStatus)
	{ 
		if(GetStatus() == CRESTATUS_TELEPORT)
			return;

		delete m_pCreStatus; 
		m_pCreStatus = 0; 
	}

	D3DXVECTOR3 loc;

	if(g_world)
	{
		if(g_world->TargetGet(targetName, loc))
			new TeleportProj(GetID(), loc);
	}
}

void Creature::Teleport(const D3DXVECTOR3 & loc)
{
	if(m_pCreStatus)
	{ 
		if(GetStatus() == CRESTATUS_TELEPORT)
			return;

		delete m_pCreStatus; 
		m_pCreStatus = 0; 
	}

	new TeleportProj(GetID(), loc);
}

//get hit, return true if we deduced hitpoints
//this creature will die if health point reaches 0
bool Creature::Hit(int amt)
{
	
	if(!CheckFlag(CRE_FLAG_INVUL))
	{
		delete m_pCreStatus; m_pCreStatus = 0;

		OBJPauseOnStateEnd(GetOBJ(), false);

		//subtract health points
		m_curHits-=amt;

		if(m_curHits < 0)
			m_curHits = 0;

		if(m_curHits > 0)
		{
			//set the status to invulnerable
			m_pCreStatus = new StatusInvulnerable(GetID(), g_hurtInvulDelay);

			//set object state
			OBJSetState(GetOBJ(), GetOuchState());

			Callback(CREMSG_HIT, amt, 0);
		}
		//DEAD!
		else if(g_world)
		{
			//we cannot allow baby Ta-Tas to be killed,
			//we must respawn
			if(GetEntityType() == ENTITY_TYPE_TATA
				&& GetSubType() == TATA_BABYTATA)
			{
				if(m_pCreStatus)
				{
					delete m_pCreStatus;
					m_pCreStatus = 0;
				}

				SetCurHit(GetMaxHit());
				Teleport(GetSpawnLoc());
			}
			else
			{
				//check to see if we have a baby tata attached
				//if so, just detach the baby tata
				if(GetEntityType() == ENTITY_TYPE_TATA)
				{
					D3DXVECTOR3 spawnLoc; 

					TaTaCommon *pTata = (TaTaCommon *)(this);

					hOBJ cObj = pTata->DetachTaTa(&spawnLoc);

					if(cObj)
					{
						//spawn Baby TaTa
						
						D3DXVECTOR3 loc; OBJGetWorldLoc(cObj, (float*)loc);
						OBJRemoveFromParent(cObj);

						Creature *pBabe = g_world->TataSpawn(g_tataTypeName[TATA_BABYTATA], loc);

						pBabe->SetOBJ(cObj);

						if(pBabe)
							pBabe->SetSpawnLoc(spawnLoc);
					}
				}

				//create corpse
				g_world->AddCorpse(this, GetDeathState());

				//poll death!
				SetFlag(ENTITY_FLAG_POLLDEATH, true);
			}
		}

		return true;
	}

	return false;
}

//stun creature
void Creature::Stun(double delay)
{
	if(!CheckFlag(CRE_FLAG_INVUL) && !CheckFlag(CRE_FLAG_STUNIMMUNE))
	{
		delete m_pCreStatus; m_pCreStatus = 0;

		m_pCreStatus = new StatusStun(GetID(), delay);
	}
}

void Creature::Freeze(hTXT frozenTxt, hMDL frozenChunk)
{
	if(!CheckFlag(CRE_FLAG_INVUL) && !CheckFlag(CRE_FLAG_NOFREEZE))
	{
		delete m_pCreStatus;

		//duplicate object
		OBJAddRef(GetOBJ());
		hOBJ blockOBJ = GetOBJ(); //OBJDuplicate(GetOBJ(), 0, 0, 0, 0);

		//OBJPauseOnStateEnd(blockOBJ, true);
		OBJPause(blockOBJ, true);

		//set texture of object to frozen texture
		int max = OBJGetNumMaterial(GetOBJ());

		for(int i = 0; i < max; i++)
			OBJSetTexture(blockOBJ, i, frozenTxt);

		//create block
		Block *newBlock = new Block;

		if(newBlock)
		{
			newBlock->SetOBJ(blockOBJ);
			newBlock->SetDebrisModel(frozenChunk); MDLAddRef(frozenChunk);
			newBlock->SetLoc(GetLoc());
			newBlock->BoundSetMin(BoundGetMin());
			newBlock->BoundSetMax(BoundGetMax());
			newBlock->BoundSetRadius(0);
			newBlock->BoundGeneratePlanes();

			if(g_world)
			{
				//poll death!
				SetFlag(ENTITY_FLAG_POLLDEATH, true);

				//play freeze sound
				CREPlaySound(12);
			}
		}
	}
}

//returns the creature status
eCreStatus Creature::GetStatus()
{
	if(m_pCreStatus)
		return m_pCreStatus->GetStatus();

	return CRESTATUS_NORMAL;
}

//get the pointer to creature status (DO NOT DELETE)
CreatureStatus * Creature::GetCreStatus()
{
	return m_pCreStatus;
}

//set creature status (DO NOT DELETE THE GIVEN POINTER AFTER CALLING THIS)
void Creature::SetCreStatus(CreatureStatus *pCreStatus)
{
	if(m_pCreStatus)
		delete m_pCreStatus;

	m_pCreStatus = pCreStatus;
}

//play sound within the creature
//ind is index in the sound list
HCHANNEL Creature::CREPlaySound(int ind)
{
	BASS_3DVECTOR pos, orient;

	memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
	memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

	return TaTaSoundPlay(ind, &pos, &orient, 0);
}

/********************************************************************
*																	*
*						Common Ta-Ta stuff							*
*																	*
********************************************************************/
TaTaCommon::~TaTaCommon() 
{
	//this will do >:)
	SetPortrait(0);

	if(m_attachedOBJ)
		OBJDestroy(&m_attachedOBJ);
}

void TaTaCommon::MoveAnim(eMoveType type)
{
	hOBJ tataOBJ = GetOBJ(); assert(tataOBJ);

	int OBJState = OBJGetCurState(tataOBJ);

	if((OBJState >= CREANIM_IDLE1 && OBJState <= CREANIM_IDLE_MAX)
		|| OBJState == CREANIM_WALK
		|| OBJState == CREANIM_BACKWALK
		|| OBJState == CREANIM_FALL)
	{
		switch(type)
		{
		case MOVE_FALL:
			OBJSetState(tataOBJ, CREANIM_FALL);
			break;

		case MOVE_NONE:
			{
				int newIdleState = Random(CREANIM_IDLE1, CREANIM_IDLE_MAX);

				if(OBJState >= CREANIM_IDLE1 && OBJState <= CREANIM_IDLE_MAX)
				{
					if(OBJIsStateEnd(tataOBJ))
						OBJSetState(tataOBJ, newIdleState);
				}
				else
					OBJSetState(tataOBJ, CREANIM_IDLE1);
			}
			break;
		
		case MOVE_WALK:
			if(CheckFlag(TATA_FLAG_BACKWALK))
			{
				OBJSetState(tataOBJ, CREANIM_BACKWALK);
				SetFlag(TATA_FLAG_BACKWALK, false);
			}
			else
				OBJSetState(tataOBJ, CREANIM_WALK);
			break;
		}
	}
	else if(OBJState == CREANIM_OUCH && OBJIsStateEnd(tataOBJ))
		OBJSetState(tataOBJ, CREANIM_IDLE1);
}

void TaTaCommon::CollideEntity(EntityCommon *pEntity, const gfxTrace & trace)
{
	//call collision callback
	//pEntity->Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)this, 0);
	Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)pEntity, (LPARAM)&trace);

	//do something if this entity is not yet ready to die
	if(!pEntity->CheckFlag(ENTITY_FLAG_POLLDEATH))
	{
		switch(pEntity->GetEntityType())
		{
		case ENTITY_TYPE_TATA:
		case ENTITY_TYPE_ENEMY:
			//complete inellastic collision
			break;

		case ENTITY_TYPE_OBJECT:
			{
				switch(pEntity->GetSubType())
				{
				case OBJECT_PLATFORM:
					//push ourself with this object
					pEntity->AddPushList(GetID());
					break;

				case OBJECT_SIGN:
					//display the sign if this tata is the current
					if(GAME_IDISEQUAL(GetID(), g_world->TataGetCurrent()))
					{
						Sign *pSign = (Sign *)pEntity;
						pSign->Show(true);
					}
					break;
				}
			}
			break;

		case ENTITY_TYPE_PROJECTILE:
			break;
		
		case ENTITY_TYPE_ITEM:
			//collect item
			pEntity->Callback(ITMMSG_COLLECT,0,0);

			//tag item death
			pEntity->SetFlag(ENTITY_FLAG_POLLDEATH, true);
			break;
		}
	}
}


void TaTaCommon::CollideWorld(hQBSP qbsp, const gfxTrace & trace)
{
	Callback(ENTITYMSG_WORLDCOLLIDE, (WPARAM)qbsp, (LPARAM)&trace);
}

//set the Ta-Ta portrait
void TaTaCommon::SetPortrait(hTXT newPort)
{
	if(m_portraitTxt)
	{ TextureDestroy(&m_portraitTxt); }

	m_portraitTxt = newPort;
}

hTXT TaTaCommon::GetPortrait()
{
	return m_portraitTxt;
}

//attaches given Ta-Ta (only use with special Ta-Ta NPC (BabyTaTa))
//NOTE: this will destroy the entity except for the object.
void TaTaCommon::AttachTaTa(TaTaCommon *pTata, int hugState)
{
	assert(pTata); assert(pTata->GetOBJ());

	m_attachedOBJ = OBJDuplicate(pTata->GetOBJ(), 0, 0,0,0);
	
	OBJAddChild(GetOBJ(), m_attachedOBJ, OBJJointGetIndex(GetOBJ(), "body"), false);

	D3DXVECTOR3 dir(0,0,-1);
	OBJSetOrientation(m_attachedOBJ, (float*)dir);
	OBJSetState(m_attachedOBJ, hugState);

	//get the spawn loc.
	m_attachedSpawnLoc = pTata->GetSpawnLoc();

	//destroy the entity
	pTata->SetFlag(ENTITY_FLAG_POLLDEATH, true);
}

bool TaTaCommon::IsAttach()
{
	return m_attachedOBJ ? true : false;
}

//detaches the Ta-Ta and sets attach object to 0,
//after calling this, you must destroy the returned
//object manually!!!
hOBJ TaTaCommon::DetachTaTa(D3DXVECTOR3 *pSpawnLoc)
{
	hOBJ retOBJ = m_attachedOBJ;

	m_attachedOBJ = 0;

	if(pSpawnLoc)
		(*pSpawnLoc) = m_attachedSpawnLoc;

	//if(retOBJ)
	//	OBJRemoveFromParent(retOBJ);

	return retOBJ;
}

const char * TaTaCommon::GetSubTypeStr() const 
{ 
	return g_tataTypeName[GetSubType()]; 
}

/********************************************************************
*																	*
*						Common Enemy stuff							*
*																	*
********************************************************************/

//Collide Entity, Enemy style
void EnemyCommon::CollideEntity(EntityCommon *pEntity, const gfxTrace & trace)
{
	//call collision callback
	Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)pEntity, (LPARAM)&trace);

	//do something if this entity is not yet ready to die
	if(!pEntity->CheckFlag(ENTITY_FLAG_POLLDEATH))
	{
		switch(pEntity->GetEntityType())
		{
		case ENTITY_TYPE_TATA:
		case ENTITY_TYPE_ENEMY:
			//complete inellastic collision
			break;

		case ENTITY_TYPE_OBJECT:
			{
				//push ourself with this object (only if it is a platform)
				if(pEntity->GetSubType() == OBJECT_PLATFORM)
					pEntity->AddPushList(GetID());
			}
			break;

		case ENTITY_TYPE_PROJECTILE:
			break;
		
		case ENTITY_TYPE_ITEM:
			break;
		}
	}
}

//Collide World, Enemy style
void EnemyCommon::CollideWorld(hQBSP qbsp, const gfxTrace & trace)
{
	Callback(ENTITYMSG_WORLDCOLLIDE, (WPARAM)qbsp, (LPARAM)&trace);
}

const char * EnemyCommon::GetSubTypeStr() const
{
	return g_enemyTypeName[GetSubType()];
}