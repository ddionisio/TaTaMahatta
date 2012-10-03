#include "tata_main.h"

#include "tata_projectiles.h"

//#include "tata_globals.h"

#include "tata_projectiles_common.h"

Projectile::Projectile(int subType, const Id & owner) : EntityCommon(this, ENTITY_TYPE_PROJECTILE, subType), m_ownerID(owner)
{ 
	SetFlag(ENTITY_FLAG_NONSOLID, true);

	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	if(!pEntity)
	{ 
		ErrorMsg("Projectile", "Invalid ID", true); 
		SetFlag(ENTITY_FLAG_POLLDEATH, true); 
	}
}

//update projectile.
RETCODE Projectile::Update() 
{
	RETCODE ret = RETCODE_SUCCESS;

	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(m_ownerID);

	if(pEntity)
		ret = Callback(ENTITYMSG_UPDATE,0,0);
	else
		//Poll ourself to death!
		SetFlag(ENTITY_FLAG_POLLDEATH, true);

	return ret; 
}

//Collide Entity, Projectile style
void Projectile::CollideEntity(EntityCommon *pEntity, const gfxTrace & trace)
{
	//do nothing if we are hitting our own owner...
	if(GAME_IDISEQUAL(GetOwner(), pEntity->GetID()))
		return;

	//call collision callback with given entity
	//tell it that the projectile hits them!  BOOM!!!!
	//pEntity->Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)this, 0);
	Callback(ENTITYMSG_ENTITYCOLLIDE, (WPARAM)pEntity, (LPARAM)&trace);

	//push depending on type
	switch(pEntity->GetEntityType())
	{
	case ENTITY_TYPE_TATA:
	case ENTITY_TYPE_ENEMY:
		//poll death
		break;

	case ENTITY_TYPE_OBJECT:
		//poll death
		break;

	case ENTITY_TYPE_PROJECTILE:
		//ignore
		break;
	
	case ENTITY_TYPE_ITEM:
		//ignore
		break;
	}
}

//Collide World, Projectile style
void Projectile::CollideWorld(hQBSP qbsp, const gfxTrace & trace)
{
	Callback(ENTITYMSG_WORLDCOLLIDE, (WPARAM)qbsp, (LPARAM)&trace);

	//poll death.
}