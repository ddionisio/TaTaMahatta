#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

PushMelee::PushMelee(const Id & owner, float pushSpd) : Projectile(PROJ_PUSHMELEE, owner)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	if(pEntity)
	{
		//don't collide with world...
		SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

		SetFlag(PROJ_FLAG_OBJINVUL, true);

		//set the speed
		m_pushSpd.MoveSetSpeed(pushSpd);

		//set the bound to be the same as the owner
		BoundSetMin(pEntity->BoundGetMin());
		BoundSetMax(pEntity->BoundGetMax());
		BoundSetRadius(pEntity->BoundGetRadius());

		D3DXVECTOR3 ownerLoc = pEntity->GetLoc();

		//set location to be at the end of the owner based on it's direction
		D3DXVECTOR3 newLoc(ownerLoc + (pEntity->GetDir()*(pEntity->BoundGetRadius()*2)));

		SetLoc(newLoc);

		SetDir(pEntity->GetDir());
	}
}

PushMelee::~PushMelee()
{
}

D3DXVECTOR3 PushMelee::GetPushVel()
{
	return D3DXVECTOR3(GetDir()*m_pushSpd.MoveUpdate(g_timeElapse));
}

int PushMelee::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			//check to see if the owner's animation is done...
			//ENTITY_FLAG_POLLDEATH
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity)
			{
				if(OBJIsStateEnd(pEntity->GetOBJ()))
				{
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}
				else
					SetVel(GetPushVel());
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}