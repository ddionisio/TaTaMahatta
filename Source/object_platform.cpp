#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_creature_tata.h"

#include "tata_globals.h"

//Object Platform Interface

Platform::Platform() : Object(OBJECT_PLATFORM)
{
	//we do not want platforms to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);
}

Platform::~Platform()
{
}

int Platform::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		//Update waypoint
		if(m_pWayPt)
		{
			D3DXVECTOR3 destPt;

			m_pWayPt->GetCurrentLinearLoc(&destPt);

			D3DXVECTOR3 dir(destPt - GetLoc());
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
				SetVel(dir);

				m_pWayPt->GotoNext();
			}
			
			//if(m_pWayPt->IsDone())
			//	RemoveWayPt();
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity)
			{
				//only 'open melee' projectiles can open us
				//and that if we are a door
				if(CheckFlag(OBJ_FLAG_DOOR)
					&& CheckFlag(OBJ_FLAG_INACTIVE))
				{
					if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE
						&& pEntity->GetSubType() == PROJ_OPENMELEE)
					{
						//just set platform to active
						SetFlag(OBJ_FLAG_INACTIVE, false);
					}
					//if it is keykey-ta
					else if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
						&& pEntity->GetSubType() == TATA_KEYKEYTA)
					{
						SetFlag(OBJ_FLAG_INACTIVE, false);
						((KeyKeyTa *)pEntity)->OpenTheDoor();
					}
				}
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;

	case ENTITYMSG_PROFILEPREP:
		SetFlag(OBJ_FLAG_INACTIVE, wParam == 0 ? true : false);
		break;
	}

	return RETCODE_SUCCESS;
}
