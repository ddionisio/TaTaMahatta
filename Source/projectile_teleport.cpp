#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature.h"

#include "tata_projectiles_common.h"

/*
/////////////////////////////////////////////////////////////////////////////
// Teleport (Teleportation!)
/////////////////////////////////////////////////////////////////////////////
class TeleportProj : public Projectile {
public:
	TeleportProj(const Id & owner, const D3DXVECTOR3 & loc);
	~TeleportProj();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
	int m_state;		//0 = init., 1 = collided, 2 = OK!
	D3DXVECTOR3 m_dest;	//destination
};*/

TeleportProj::TeleportProj(const Id & owner, const D3DXVECTOR3 & loc)
: Projectile(PROJ_TELEPORT, owner), m_state(0), m_dest(loc)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

	if(pEntity
		&& (pEntity->GetEntityType() == ENTITY_TYPE_TATA
		|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY))
	{
		BoundSetMin(pEntity->BoundGetMin());
		BoundSetMax(pEntity->BoundGetMax());
		BoundSetRadius(pEntity->BoundGetRadius());

		//CRESTATUS_TELEPORT
		Creature *pCre = (Creature*)pEntity;
		CreatureStatus *pNewCreStat = new StatusTeleport(GetOwner(), m_dest);
		pCre->SetCreStatus(pNewCreStat);
	}
}

TeleportProj::~TeleportProj()
{
}

int TeleportProj::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			//check to see if the owner's animation is done...
			//ENTITY_FLAG_POLLDEATH
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity
				&& (pEntity->GetEntityType() == ENTITY_TYPE_TATA
				|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY))
			{
				SetLoc(m_dest);
				//SetVel(pEntity->GetVel());

				switch(m_state)
				{
				case 0:
					m_state = 2;
					break;
				case 1:
					{
						Creature *pCre = (Creature*)pEntity;

						if(pCre->GetStatus() == CRESTATUS_TELEPORT)
							pCre->SetCreStatus(0);

						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
					break;
				case 2:
					{
						Creature *pCre = (Creature*)pEntity;

						if(pCre->GetStatus() == CRESTATUS_TELEPORT)
						{
							((StatusTeleport *)pCre->GetCreStatus())->GO();
						}

						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
					break;
				}
			}
			else
				SetFlag(ENTITY_FLAG_POLLDEATH, true);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity && !pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
				m_state = 1;
		}
		break;

	case ENTITYMSG_WORLDCOLLIDE:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}