
#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

/*
/////////////////////////////////////////////////////////////////////////////
// Open Melee (for Key Key Ta)
/////////////////////////////////////////////////////////////////////////////
class OpenMelee : public Projectile {
public:
	OpenMelee(const Id & owner);
	~OpenMelee();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);
private:
};*/

OpenMelee::OpenMelee(const Id & owner) : Projectile(PROJ_OPENMELEE, owner)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	//don't collide with world...
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	if(pEntity)
	{
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

OpenMelee::~OpenMelee()
{
}

int OpenMelee::Callback(unsigned int msg, unsigned int wParam, int lParam)
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
				hOBJ obj = pEntity->GetOBJ();

				if(OBJIsStateEnd(obj))
				{
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}
				else
					SetVel(GetDir());
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