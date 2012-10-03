#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

PullPower::PullPower(const Id & owner, const D3DXVECTOR3 & bound, float length, float spd)
 : Projectile(PROJ_PULLPOWER, owner), m_maxLength(length), m_curLength(0)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	if(pEntity)
	{
		//don't collide with world...
		SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

		SetFlag(PROJ_FLAG_OBJINVUL, true);

		SetFlag(ENTITY_FLAG_NONSOLID_COLLIDE, true);

		//set the speed
		m_spd.MoveSetSpeed(spd);

		////////////////////////////////////////////////////////
		//create the bound
		BoundSetMin(-bound);
		BoundSetMax(bound);

		float len = D3DXVec3Length(&bound);
		BoundSetRadius(len);
	}
}

PullPower::~PullPower()
{
}

int PullPower::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity)
			{
				////////////////////////////////////////////////////////
				//set location
				SetLoc(pEntity->GetLoc()+(m_maxLength*pEntity->GetDir()));
				
				//set direction
				SetDir(-pEntity->GetDir());

				//set vel.
				SetVel(-m_maxLength*pEntity->GetDir());

				//move it
				/*float spd = m_spd.MoveUpdate(g_timeElapse);

				m_curLength += spd;

				if(m_curLength >= m_maxLength)
				{
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}*/
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			//sigh...
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity->CheckFlag(ENTITY_FLAG_PULLABLE))
			{
				pEntity->SetLoc(pEntity->GetLoc());
				pEntity->SetVel(pEntity->GetVel()
							+(m_spd.MoveUpdate(g_timeElapse)*GetDir()));
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}