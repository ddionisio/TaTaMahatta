#include "tata_main.h"

#include "tata_basic.h"

#include "tata_collision.h"

#include "tata_stage_select.h"

#include "tata_globals.h"

EntityCommon::EntityCommon(void *thisPtr, eEntityType type, int subType) 
: GameID(thisPtr), m_type(type), m_subType(subType), m_obj(0), m_loc(0,0,0), m_dir(0,0,-1), m_vel(0,0,0), 

m_bPushed(false),

m_tShadow(1),

m_saveCollectReq("none"),
m_saveStage(-1), m_saveLevel(-1)
{
	m_bcollideWith = false;
	m_LastCollided.counter = -1;
	
	memset(&m_gfxTrace, 0, sizeof(gfxTrace));
	m_gfxTrace.t = 1;
	m_gfxTrace.leafIndex = -1;

	CollisionAddEntity(GetID());
}

EntityCommon::~EntityCommon()
{
	CollisionRemoveEntity(GetID());

	if(m_obj)
	{ OBJDestroy(&m_obj); m_obj = 0; }
}

void EntityCommon::SetOBJ(hOBJ obj)
{
	if(m_obj)
		OBJDestroy(&m_obj);

	m_obj = obj;
}

void EntityCommon::SetLoc(const D3DXVECTOR3 & v) 
{ 
	m_loc = v; 
	m_bPushed = false;
	
	if(m_obj) 
	{ 
		OBJTranslate(m_obj, (float*)m_loc, false);

		BoundTranslate(m_loc);
	}
	else if(g_world && BoundGetModelInd() > 0)
	{
		QBSPModelTranslate(g_world->GetMap(), BoundGetModelInd(), (float*)m_loc, false);
	}
}

void EntityCommon::SetDir(const D3DXVECTOR3 & v) 
{ 
	m_dir = v; 
	
	if(m_obj) 
	{ 
		OBJSetOrientation(m_obj, (float*)m_dir); 
	}
}

bool EntityCommon::SetVel(const D3DXVECTOR3 & v)
{ 
	if(!m_bPushed)
	{
		m_vel = v; 
		return true;
	}

	return false;
}

void EntityCommon::Show(bool bShow)
{
	if(bShow)
	{
		if(m_obj)
			OBJActivate(m_obj, true);
		else if(g_world && BoundGetModelInd() > 0)
			QBSPModelEnable(g_world->GetMap(), BoundGetModelInd(), true);
	}
	else
	{
		if(m_obj)
			OBJActivate(m_obj, false);
		else if(g_world && BoundGetModelInd() > 0)
			QBSPModelEnable(g_world->GetMap(), BoundGetModelInd(), false);
	}
}

//add an entity who will push me.
void EntityCommon::AddPushList(const Id & ID)
{
	//check to see if ID is already added
	for(list<Id>::iterator i = m_PushList.begin(); i != m_PushList.end(); i++)
	{
		if(GAME_IDISEQUAL(ID, *i))
			return;
	}

	m_PushList.push_back(ID);
}

void EntityCommon::ExecutePushList()
{
	EntityCommon *pEntity;

	for(list<Id>::iterator i = m_PushList.begin(); i != m_PushList.end(); i++)
	{
		pEntity = (EntityCommon*)IDPageQuery(*i);

		if(pEntity)
		{
			pEntity->m_vel += m_vel;

			if(m_vel.y > 0)
				pEntity->m_vel.y = m_vel.y;
		}
	}

	m_PushList.clear();
}

void EntityCommon::CollAddEntity(const Id & ID)
{
	//check to see if we already have this entity
	/*for(list<Id>::iterator i = m_collideList.begin(); i != m_collideList.end(); i++)
	{
		if(GAME_IDISEQUAL(*i, ID))
			return;
	}*/

	m_collideList.push_back(ID);
}

void EntityCommon::CollClear()
{
	m_collideList.clear();
}

const char * EntityCommon::GetEntityTypeStr() const
{
	switch(GetEntityType())
	{
	case ENTITY_TYPE_PROJECTILE:
		return "projectile";

	case ENTITY_TYPE_ITEM:
		return "item";

	case ENTITY_TYPE_OBJECT:
		return "object";

	case ENTITY_TYPE_TATA:
		return "tata";

	case ENTITY_TYPE_ENEMY:
		return "enemy";
	}

	return 0;
}

void EntityCommon::ProfileSetReqCollection(const char *str)
{
	if(str && str[0] != 0)
		m_saveCollectReq = str;
	else
		m_saveCollectReq = "none";
}

void EntityCommon::ProfileSetReqStageLevel(int stage, int level)
{
	m_saveStage = stage;
	m_saveLevel = level;
}

//this will do stuff depending on given profile
void EntityCommon::ProfileCheckReq(hCFG profile)
{
	if(stricmp(m_saveCollectReq.c_str(), "none") != 0
		|| (m_saveStage >= 0 && m_saveLevel >= 0))
	{
		bool bCollection=true, bStageLevel=true;

		if(profile)
		{
			if(stricmp(m_saveCollectReq.c_str(), "none") != 0)
				bCollection = CfgGetItemInt(profile, PROFILE_COLLECT_SECTION, m_saveCollectReq.c_str()) == 1 ? true : false;
			
			if(m_saveStage >= 0 && m_saveLevel >= 0)
				bStageLevel = StageSelectGetLevelState(m_saveStage, m_saveLevel) != ST_LOCK ? true : false;
		}
		else
		{ bCollection = bStageLevel = false; }

		Callback(ENTITYMSG_PROFILEPREP, (WPARAM)(bCollection && bStageLevel), 0);
	}
}

#define SPD_TOLER	0.001f

float Move::MoveUpdate(double timeFactor) //get the distance over time...sort of...
{
	float time   = (timeFactor*SPD_TOLER);

	return m_spd*time;
}