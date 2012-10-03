#include "tata_main.h"

#include "tata_creature.h"
#include "tata_creature_tata.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Goal::Goal() : Object(OBJECT_GOAL), m_goalType(-1), m_bCollide(false)
{
	m_collideWith.counter = -1;

	m_collideID = "none";

	//we do not want goals to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);
}

Goal::~Goal()
{
}

char *g_goalStr[Goal::GOAL_MAX] = {"tatahome", "rescue", "gather", "destroy"};

int Goal::GetGoalType(const char *typeName)
{
	for(int i = 0; i < GOAL_MAX; i++)
	{
		if(stricmp(g_goalStr[i], typeName) == 0)
			return i;
	}

	return -1;
}

void Goal::SetGoal(int type)
{
	m_goalType = type;
}

void Goal::SetRequiredID(const char *IDName)
{
	m_collideID = IDName;
}

Id Goal::GetCollideWith()
{
	return m_collideWith;
}

int Goal::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		switch(m_goalType)
		{
		case GOAL_DESTROY:
			{
				if(stricmp(m_collideID.c_str(), "none") != 0)
				{
					EntityCommon *pEntity = (EntityCommon*)IDPageQuery(CollisionGetEntity(m_collideID.c_str()));

					//if this entity does not exist, then increment
					//goal (meaning this entity died or something)
					//set required ID to none
					//kill ourself
					if(!pEntity)
					{
						//add to goal
						if(g_world)
						{
							g_world->HUDSetGoalMinFixed(g_world->HUDGetGoalMinFixed()+1);
						}

						m_collideID = "none";

						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
				}
			}
			break;
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			switch(m_goalType)
			{
			case GOAL_TATAHOME:	//get Ta-Ta home, Ta-Ta will be removed once home
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA)
				{
					pEntity->SetFlag(ENTITY_FLAG_GOAL, true);
				}
				
				break;
			case GOAL_RESCUE:	//rescue little Ta-Tas
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA)
				{
					TaTaCommon *pTata = (TaTaCommon *)pEntity;

					//make sure something is attached
					if(pTata->IsAttach())
					{
						hOBJ obj = pTata->DetachTaTa();

						D3DXVECTOR3 loc; OBJGetLoc(obj, (float*)loc);

						OBJRemoveChild(pTata->GetOBJ(), obj);

						OBJTranslate(obj, (float*)loc, false);
						OBJSetOrientation(obj, (float*)pTata->GetDir());

						g_world->AddExitObj(obj, CREANIM_VICTORY, CREANIM_FALL);

						OBJDestroy(&obj);

						//add to goal
						if(g_world)
						{
							g_world->HUDSetGoalMinFixed(g_world->HUDGetGoalMinFixed()+1);
						}
					}
				}
				break;
			case GOAL_GATHER:	//gather something based on ID
				if(!m_bCollide)
				{
					//make sure it matches
					if(stricmp(m_collideID.c_str(), "none") == 0
						|| strcmp(m_collideID.c_str(), pEntity->GetName()) == 0)
					{
						if(!GAME_IDISEQUAL(m_collideWith, pEntity->GetID()))
						{
							m_collideWith = pEntity->GetID();
							m_bCollide = true;

							pEntity->SetFlag(ENTITY_FLAG_GOAL, true);
						}
					}
				}
				//make sure it matches
				/*if(stricmp(m_collideID.c_str(), "none") == 0
					|| strcmp(m_collideID.c_str(), pEntity->GetName()) == 0)
				{
					pEntity->SetFlag(ENTITY_FLAG_GOAL, true);
				}*/
				break;
			}
		}
		break;

	case ENTITYMSG_ENTITYNOCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity->GetEntityType() == ENTITY_TYPE_TATA)
			{
				pEntity->SetFlag(ENTITY_FLAG_GOAL, false);
			}
			/*else if(stricmp(m_collideID.c_str(), "none") == 0
				|| strcmp(m_collideID.c_str(), pEntity->GetName()) == 0)
			{
				pEntity->SetFlag(ENTITY_FLAG_GOAL, false);
			}*/

			if(m_bCollide)
			{
				if(GAME_IDISEQUAL(m_collideWith, pEntity->GetID()))
				{
					m_bCollide = false;
					m_collideWith.counter = -1;

					if(stricmp(m_collideID.c_str(), "none") == 0
						|| strcmp(m_collideID.c_str(), pEntity->GetName()) == 0)
					{
						pEntity->SetFlag(ENTITY_FLAG_GOAL, false);
					}
				}
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}