#ifndef _tata_projectiles_h
#define _tata_projectiles_h

#include "tata_ID.h"
#include "tata_basic.h"
#include "tata_bound.h"
#include "tata_waypoint.h"

#include "tata_collision.h"

//
// This file includes all stuff about projectiles
//

//this is the Abstract Base Class for Projectiles

class Projectile : public EntityCommon 
{
public:
	Projectile(int subType, const Id & owner);

	virtual ~Projectile() {}

	Id GetOwner() { return m_ownerID; } //returns the owner of this projectile.

	//change the owner
	void SetOwner(const Id & newOwner) { m_ownerID = newOwner; }

	//Collide Entity, Projectile style
	void CollideEntity(EntityCommon *pEntity, const gfxTrace & trace);

	//Collide World, Projectile style
	void CollideWorld(hQBSP qbsp, const gfxTrace & trace);

	RETCODE Update();		//update projectile.

private:
	Id		m_ownerID;		//owner of projectile.
};

#endif