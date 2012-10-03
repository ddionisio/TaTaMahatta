#include "tata_main.h"

#include "tata_object.h"

#include "tata_globals.h"

#include "tata_object_common.h"

//all base object stuff

Object::Object(int subType) : EntityCommon(this, ENTITY_TYPE_OBJECT, subType), m_script(0)
{
	//all objects cast shadow
	SetFlag(ENTITY_FLAG_SHADOW, true);
}

Object::~Object()
{
	RemoveScript();

	//disable the model in the map
	if(BoundGetModelInd() != -1 && g_world->GetMap())
		QBSPModelEnable(g_world->GetMap(), BoundGetModelInd(), false);
}

// if section is NULL, then
// the entire file is parsed.
// format of section: [<section>_begin]
// 				      ... // code
//					  [<section>_end]
RETCODE Object::LoadScript(const char *filename, const char *section)
{
	RemoveScript();

	char beginStr[MAXCHARBUFF], endStr[MAXCHARBUFF];

	strcpy(beginStr, "[");
	strcat(beginStr, section);
	strcat(beginStr, "_begin]");

	strcpy(endStr, "[");
	strcat(endStr, section);
	strcat(endStr, "_end]");

	m_script = ScriptLoad(&GetID(), filename, beginStr, endStr);

	return RETCODE_SUCCESS;
}

//destroys current script
void Object::RemoveScript()
{
	if(m_script)
	{ 
		ScriptDestroy(&GetID(), &m_script);

		m_script = 0;
	}
}

RETCODE Object::Update()
{
	RETCODE ret = RETCODE_SUCCESS;

	//make sure we are active!
	if(!CheckFlag(OBJ_FLAG_INACTIVE))
	{
		//Update waypoint
		/*if(m_pWayPt)
		{
			D3DXVECTOR3 newPt;

			m_pWayPt->GetCurrentLinearLoc(&newPt);

			if(SetVel(newPt - GetLoc()))
				m_pWayPt->Update();

			if(m_pWayPt->IsDone())
				RemoveWayPt();
		}*/

		//Update script
		if(m_script)
		{
			if(ScriptUpdate(&GetID(), m_script) == RETCODE_BREAK)
				RemoveScript();
		}

		ret = Callback(ENTITYMSG_UPDATE,0,0);
	}

	return ret;
}

void Object::CollideEntity(EntityCommon *pEntity, const gfxTrace & trace)
{
	//pEntity->Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)this, 0);
	RETCODE ret = Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)pEntity, (LPARAM)&trace);

	//push depending on type
	switch(pEntity->GetEntityType())
	{
	case ENTITY_TYPE_TATA:
	case ENTITY_TYPE_ENEMY:
		//push the entity
		//only if I'm a platform.
		if(GetSubType() == OBJECT_PLATFORM)
			AddPushList(pEntity->GetID());
		break;

	case ENTITY_TYPE_OBJECT:
		{
			if(GetSubType() == OBJECT_BLOCK)
			{
				//if I'm a block and I collided with a platform
				//then let myself be pushed by that platform
				if(pEntity->GetSubType() == OBJECT_PLATFORM)
					pEntity->AddPushList(GetID());	
			}
			else if(GetSubType() == OBJECT_PLATFORM)
			{
				//if I'm a platform and I collided with a block
				//then let's push that SOB
				if(pEntity->GetSubType() == OBJECT_BLOCK)
					AddPushList(pEntity->GetID());
			}
		}
		break;

	case ENTITY_TYPE_PROJECTILE:
		//tag projectile death
		if(!pEntity->CheckFlag(PROJ_FLAG_OBJINVUL))
			pEntity->SetFlag(ENTITY_FLAG_POLLDEATH, true);
		break;
	
	case ENTITY_TYPE_ITEM:
		//ignore
		break;
	}
}

void Object::CollideWorld(hQBSP qbsp, const gfxTrace & trace)
{
}
