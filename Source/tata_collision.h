#ifndef _tata_collision_h
#define _tata_collision_h

/////////////////////////////////////
// Name:	CollisionDestroyBoundBuff
// Purpose:	delete the bound buff
// Output:	destroys bound buffer
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionDestroyBoundBuff();

/////////////////////////////////////
// Name:	CollisionSetGravSpd
// Purpose:	set the 'gravity' velocity
// Output:	down speed set
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionSetGravSpd(float f);

/////////////////////////////////////
// Name:	CollisionAddEntity
// Purpose:	add entity to the collision
//			list.
// Output:	added to list
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionAddEntity(const Id & ID);

/////////////////////////////////////
// Name:	CollisionRemoveEntity
// Purpose:	remove entity from list
// Output:	dead
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionRemoveEntity(const Id & ID);

/////////////////////////////////////
// Name:	CollisionGetEntity
// Purpose:	look for the entity with the
//			given name.
// Output:	none
// Return:	the ID, ID is invalid if
//			name does not match.
/////////////////////////////////////
PUBLIC Id CollisionGetEntity(const char *name);

/////////////////////////////////////
// Name:	CollisionGetEntityDoodad
// Purpose:	look for the entity with the
//			given name.
// Output:	none
// Return:	the ID, ID is invalid if
//			name does not match.
/////////////////////////////////////
PUBLIC Id CollisionGetEntityDoodad(const char *name);

/////////////////////////////////////
// Name:	CollisionEntitiesDoVelocity
// Purpose:	update all entity with velocity.
//			this will apply push-velocity
//			and 'gravity'.  Once done, push
//			velocity is set back to zero
// Output:	entity current velocity set
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionEntitiesDoVelocity();

/////////////////////////////////////
// Name:	CollisionEntitiesUpdate
// Purpose:	nothing to do with collision.
//			But anyway, updates all entities.
// Output:	stuff updated
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionEntitiesUpdate();

/////////////////////////////////////
// Name:	CollisionDestroyAllEntities
// Purpose:	this will destroy all entites
//			Only use this on world destroy
// Output:	all entities dead
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionDestroyAllEntities();

/////////////////////////////////////
// Name:	CollisionTaTaVictory
// Purpose:	set all Ta-Ta to victory
//			dance
// Output:	all Ta-Tas happy
// Return:	none
/////////////////////////////////////
PUBLIC void CollisionTaTaVictory();

/////////////////////////////////////
// Name:	CollisionExecute
// Purpose:	collide given data and
//			stuff happens.
//			NOTE: you might want to use
//			vel as temporary pass.
// Output:	pCollide is set to new leaf
//			and ptInOut set to right place.
// Return:	true if we moved
/////////////////////////////////////
PUBLIC void CollisionExecute(hQBSP qbsp);

/////////////////////////////////////
// Name:	CollisionPrepare
// Purpose:	prepare the entities based
//			on profile
// Output:	enable/disable doodads,
//			activate/deactivate entities
// Return:	none
/////////////////////////////////////
PUBLIC void CollisionPrepare(hCFG profile);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CollisionBltShadows();

#endif