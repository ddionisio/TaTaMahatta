#include "tata_main.h"

#include "tata_basic.h"

#include "tata_object_common.h"

#include "tata_collision.h"

#include "tata_globals.h"

#define SHADOW_OFS		0.01f

const D3DXVECTOR3 g_zeroVel(0,0,0);
const D3DXVECTOR3 g_shadowVel(0,-1000,0);

#define MAX_NUM_BRUTE_FORCE		10 //max number of objects for using brute-force

typedef enum {
	AXIS_INVALID,
	AXIS_X,
	AXIS_Y,
	AXIS_Z
} eAxis;

//This is the list of all entities used by the game
//This is later used for collision
//NOTE: MAKE SURE THIS ENTITY HAS THE EntityCommon INHERITED!!!!
struct EntityBoundDat {
	Id    ID;
	float val;
	bool  bOpen;
};

list< Id > g_entityList;

list< Id > g_doodadList;	//this is only used for update
							//collision is not involved!

EntityBoundDat g_entityBounds[8192];	//big giant bound data
int g_entityBoundsSize=8192;

bool g_bNotDone = false;

Move	   g_spdDown;		//our 'gravity'

//if bDestroy, then just destroy the entity bounds
PRIVATE void _EntityBoundRebuild(bool bDestroy)
{
	/*if(g_entityBounds)
	{ delete [] g_entityBounds; g_entityBounds = 0; g_entityBoundsSize = 0; }

	if(!bDestroy)
	{
		g_entityBoundsSize = g_entityList.size()*20;
		g_entityBounds = new EntityBoundDat[g_entityBoundsSize];
	}*/
}

/////////////////////////////////////
// Name:	CollisionSetGravSpd
// Purpose:	set the 'gravity' velocity
// Output:	down speed set
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionSetGravSpd(float f)
{
	g_spdDown.MoveSetSpeed(f);
}

/////////////////////////////////////
// Name:	CollisionAddEntity
// Purpose:	add entity to the collision
//			list.
// Output:	added to list
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionAddEntity(const Id & ID)
{//OBJECT_DOODAD
	//put in type order
	EntityCommon *pThisEntity = (EntityCommon*)IDPageQuery(ID);

	if(pThisEntity)
	{
		EntityCommon *pEntity;

		//add to collision list or doodad
		list<Id> & theList 
			= (pThisEntity->GetEntityType() == ENTITY_TYPE_OBJECT 
			&& pThisEntity->GetSubType() == OBJECT_DOODAD) 
			? g_doodadList : g_entityList;

		for(list<Id>::iterator i = theList.begin(); i != theList.end(); i++)
		{
			pEntity = (EntityCommon *)IDPageQuery(*i);

			if(pEntity)
			{
				if(pThisEntity->GetEntityType() <= pEntity->GetEntityType())
				{
					//compare sub type
					if(pThisEntity->GetEntityType() == pEntity->GetEntityType()
						&& pThisEntity->GetSubType() <= pEntity->GetSubType())
					{
						//not cool.
						continue;
					}
					
					//insert before this element
					theList.insert(i, ID);

					//done
					return;
				}
			}
		}

		theList.push_back(ID);
	}

	_EntityBoundRebuild(false);
}

/////////////////////////////////////
// Name:	CollisionRemoveEntity
// Purpose:	remove entity from list
// Output:	dead
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionRemoveEntity(const Id & ID)
{
	//search for this ID in the list and delete it
	list< Id >::iterator it, itNext;

	for(it = g_entityList.begin(); it != g_entityList.end();)
	{
		itNext = it;
		itNext++;

		if(GAME_IDISEQUAL(ID, *it))
		{
			g_entityList.erase(it);
			return;
		}

		it = itNext;
	}

	for(it = g_doodadList.begin(); it != g_doodadList.end();)
	{
		itNext = it;
		itNext++;

		if(GAME_IDISEQUAL(ID, *it))
		{
			g_doodadList.erase(it);
			return;
		}

		it = itNext;
	}

	_EntityBoundRebuild(false);
}

/////////////////////////////////////
// Name:	CollisionGetEntity
// Purpose:	look for the entity with the
//			given name.
// Output:	none
// Return:	the ID, ID is invalid if
//			name does not match.
/////////////////////////////////////
PUBLIC Id CollisionGetEntity(const char *name)
{
	EntityCommon *pEntity;

	for(list< Id >::iterator it = g_entityList.begin(); it != g_entityList.end(); it++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			if(stricmp(pEntity->GetName(), name) == 0)
				return *it;
		}
	}

	Id badID = {0,-1};

	return badID;
}

/////////////////////////////////////
// Name:	CollisionGetEntityDoodad
// Purpose:	look for the entity with the
//			given name.
// Output:	none
// Return:	the ID, ID is invalid if
//			name does not match.
/////////////////////////////////////
PUBLIC Id CollisionGetEntityDoodad(const char *name)
{
	EntityCommon *pEntity;

	for(list< Id >::iterator it = g_doodadList.begin(); it != g_doodadList.end(); it++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			if(stricmp(pEntity->GetName(), name) == 0)
				return *it;
		}
	}

	Id badID = {0,-1};

	return badID;
}

/////////////////////////////////////
// Name:	CollisionEntitiesDoVelocity
// Purpose:	update all entity with velocity.
//			this will apply push-velocity
//			and 'gravity'.  Once done, push
//			velocity is set back to zero
// Output:	entity current velocity set
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionEntitiesDoVelocity()
{
	EntityCommon *pEntity;

	list< Id >::iterator it;

	D3DXVECTOR3 gravVel(0,-g_spdDown.MoveUpdate(g_timeElapse),0);

	for(it = g_entityList.begin(); it != g_entityList.end(); it++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			/*if(pEntity->CheckFlag(ENTITY_FLAG_ONPLATFORM))
			{
				pEntity->m_bPushed = false;
				pEntity->SetFlag(ENTITY_FLAG_ONPLATFORM, false);
			}
			else*/ if(pEntity->GetVel() == g_zeroVel)
			{
				pEntity->m_bPushed = false;

				if(pEntity->Callback(ENTITYMSG_ALLOWGRAVITY,0,0))
					pEntity->SetVel(gravVel);
			}
			//else
			//	pEntity->m_bPushed = true;
		}
	}
}

PRIVATE void _CollisionEntitiesUpdate(list< Id > & theList, bool bSetGoal)
{
	EntityCommon *pEntity;

	int minGoal = 0;

	list< Id >::iterator it, itNext;

	for(it = theList.begin(); it != theList.end();)
	{
		itNext = it; itNext++;

		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			if(pEntity->CheckFlag(ENTITY_FLAG_GOAL))
				minGoal++;

			//check to see if this entity is ready to die!
			if(pEntity->CheckFlag(ENTITY_FLAG_POLLDEATH))
			{
				//remove from list
				theList.erase(it);

				//call death message...
				pEntity->Callback(ENTITYMSG_DEATH, 0, 0);

				//delete the entity
				delete pEntity;
			}
			else
			{
				pEntity->Update();

				//push the entities
				pEntity->ExecutePushList();
			}
		}
		else
			//remove from list
			theList.erase(it);

		it = itNext;
	}

	if(g_world && bSetGoal)
		g_world->HUDSetGoalMin(minGoal);
}

/////////////////////////////////////
// Name:	CollisionEntitiesUpdate
// Purpose:	nothing to do with collision.
//			But anyway, updates all entities.
// Output:	stuff updated
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionEntitiesUpdate()
{
	_CollisionEntitiesUpdate(g_entityList, true);
	_CollisionEntitiesUpdate(g_doodadList, false);
}

PRIVATE inline void _CollisionDestroyAllEntities(list< Id > theList)
{
	EntityCommon *pEntity;

	list< Id >::iterator it, itNext;

	for(it = theList.begin(); it != theList.end();)
	{
		itNext = it;
		itNext++;

		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			theList.erase(it);

			delete pEntity;
		}

		it = itNext;
	}
}

/////////////////////////////////////
// Name:	CollisionDestroyAllEntities
// Purpose:	this will destroy all entites
//			Only use this on world destroy
// Output:	all entities dead
// Return:	none
/////////////////////////////////////
PROTECTED void CollisionDestroyAllEntities()
{
	_CollisionDestroyAllEntities(g_entityList);
	_CollisionDestroyAllEntities(g_doodadList);

	_EntityBoundRebuild(true);
}

/////////////////////////////////////
// Name:	CollisionTaTaVictory
// Purpose:	set all Ta-Ta to victory
//			dance
// Output:	all Ta-Tas happy
// Return:	none
/////////////////////////////////////
PUBLIC void CollisionTaTaVictory()
{
	EntityCommon *pEntity;

	for(list< Id >::iterator it = g_entityList.begin(); it != g_entityList.end(); it++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity && pEntity->GetEntityType() == ENTITY_TYPE_TATA)
		{
			Creature *pCre = (Creature*)pEntity;
			OBJSetState(pCre->GetOBJ(), CREANIM_VICTORY);
		}
	}
}

#define PUSH_STUCK_CRAP		1.5f//1.0f//6

//collide 1 to 2
void __fastcall _CollidePair(hQBSP qbsp, 
							 EntityCommon *pEntity1,
							 EntityCommon *pEntity2)
{
	//do not collide with non-solid entities...
	if(pEntity2->CheckFlag(ENTITY_FLAG_NONSOLID))
	{
		//we make exceptions for some...
		if(!pEntity1->CheckFlag(ENTITY_FLAG_NONSOLID_COLLIDE))
		{
			return;
		}
	}

	//do not collide with own owner, if projectile
	if(pEntity1->GetEntityType() == ENTITY_TYPE_PROJECTILE)
	{
		Projectile *pProj = (Projectile *)pEntity1;
		if(GAME_IDISEQUAL(pProj->GetOwner(), pEntity2->GetID()))
			return;
	}

	bool ret;

	gfxTrace traceTemp={0}; traceTemp.t = 1;

	Bound *pBound1 = dynamic_cast<Bound *>(pEntity1);

	//D3DXVECTOR3 tempLoc(pEntity2->GetLoc());
	//pEntity2->SetLoc(pEntity2->GetLoc()+pEntity2->GetVel());

	ret = pEntity2->BoundCollide(qbsp, pEntity2->GetLoc(), *pBound1,
			pEntity1->GetLoc(), pEntity1->GetVel(), 
			&traceTemp, 1);
	
	if(ret && traceTemp.t <= pEntity1->m_gfxTrace.t)
	{
		pEntity1->m_gfxTrace = traceTemp;
		pEntity1->m_bcollideWith = true;
		pEntity1->m_LastCollided = pEntity2->GetID();

		//do something special upon collision...
		pEntity2->CollideEntity(pEntity1, traceTemp);

		if(pEntity1->CheckFlag(ENTITY_FLAG_NONSOLID))
			pEntity1->CollideEntity(pEntity2, traceTemp);
	}
	else
		pEntity1->Callback(ENTITYMSG_ENTITYNOCOLLIDE, (WPARAM)pEntity2, 0);
}

PRIVATE inline void _CollisionEntities(hQBSP qbsp, EntityCommon *pEntity)
{
	//collide with entities
	EntityCommon *pEntity2;

	for(list<Id>::iterator itE = pEntity->m_collideList.begin(); itE != pEntity->m_collideList.end(); itE++)
	{
		pEntity2 = (EntityCommon *)IDPageQuery(*itE);

		if(pEntity2)
			_CollidePair(qbsp, pEntity, pEntity2);
	}
}

//collide with map and entities in the entity list
PRIVATE void _CollisionMap(hQBSP qbsp, EntityCommon *pEntity)
{
	D3DXVECTOR3 intLoc;

	if(pEntity->GetVel() != g_zeroVel 
		&& !pEntity->CheckFlag(ENTITY_FLAG_NOWORLDCOLLISION))
	{
		intLoc = pEntity->GetLoc() + pEntity->GetVel();

		QBSPCollision(qbsp, (float*)pEntity->BoundGetMin(), 
			(float*)pEntity->BoundGetMax(), (float*)pEntity->GetLoc(), 
			(float*)intLoc,  &pEntity->m_gfxTrace, 1, -1);

		if(pEntity->m_gfxTrace.t < 1)
			pEntity->CollideWorld(qbsp, pEntity->m_gfxTrace);
	}

	_CollisionEntities(qbsp, pEntity);
}

PRIVATE void _CollisionMapShadow(hQBSP qbsp, EntityCommon *pEntity1)
{
	if(pEntity1->CheckFlag(ENTITY_FLAG_SHADOW)
		&& !pEntity1->CheckFlag(ENTITY_FLAG_NOWORLDCOLLISION)
		&& pEntity1->BoundGetRadius() > 0)
	{
		Bound sBound;
		sBound.BoundSetMin(D3DXVECTOR3(pEntity1->BoundGetMin().x,-SHADOW_OFS, pEntity1->BoundGetMin().z));
		sBound.BoundSetMax(D3DXVECTOR3(pEntity1->BoundGetMax().x,SHADOW_OFS, pEntity1->BoundGetMax().z));

		gfxTrace trace;

		D3DXVECTOR3 intLoc = pEntity1->GetLoc() + g_shadowVel;

		QBSPCollision(qbsp, (float*)sBound.BoundGetMin(), 
			(float*)sBound.BoundGetMax(), (float*)pEntity1->GetLoc(), 
			(float*)intLoc,  &trace, 1, -1);

		pEntity1->m_tShadow = trace.t;

		//collide with entities
		EntityCommon *pEntity2;

		D3DXVECTOR3 vel(g_shadowVel);

		for(list<Id>::iterator itE = pEntity1->m_collideList.begin(); itE != pEntity1->m_collideList.end(); itE++)
		{
			pEntity2 = (EntityCommon *)IDPageQuery(*itE);

			if(pEntity2 && !pEntity2->CheckFlag(ENTITY_FLAG_NONSOLID))
			{
				memset(&trace, 0, sizeof(gfxTrace));
				trace.t = 1;

				if(pEntity2->BoundCollide(qbsp, pEntity2->GetLoc(), sBound,
					pEntity1->GetLoc(), vel, 
					&trace, 1) 
					&& trace.t < pEntity1->m_tShadow)
					pEntity1->m_tShadow = trace.t;
			}
		}

		if(pEntity1->m_tShadow < 1)
		{
			pEntity1->m_shadowLoc = pEntity1->GetLoc() + (pEntity1->m_tShadow*g_shadowVel);
		}
	}
}

#define TOLER_COLLIDE 0.003f//0.003f//0.55f//0.54f//0.6f//0.1f//

//calculate new point and determine if we need to move again.
//returns 1 if done with collision (means given entity
//will not collide with others).  (it can still be collided by others.)
PRIVATE void _CollisionSet(hQBSP qbsp, EntityCommon *pEntity)
{
	float nd;

	float mrT = pEntity->m_gfxTrace.t;

	D3DXVECTOR3 ptInOut, vel;

	if(pEntity->GetVel() == g_zeroVel)
		goto DONE;

	if(mrT < 1)
	{
		ptInOut = pEntity->GetLoc();
		vel     = pEntity->GetVel();

		D3DXVECTOR3 norm(pEntity->m_gfxTrace.norm);

		D3DXVECTOR3 newVel(0,0,0);

		////////////////////////////////////
		// get the sliding vector
		if(mrT > 0)
		{
			if(!(vel.x == 0 && vel.y < 0 && vel.z == 0))
			{
				nd = D3DXVec3Dot(&norm, &vel);

				newVel.x = vel.x - norm.x*nd; 
				newVel.y = vel.y - norm.y*nd;
				newVel.z = vel.z - norm.z*nd;
			}
		}
		////////////////////////////////////
#if 1
		mrT -= TOLER_COLLIDE;

		if(mrT < 0
			&& !pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
		{
			gfxTrace traceTmp=pEntity->m_gfxTrace;

			pEntity->SetVel(vel*mrT);
			pEntity->m_gfxTrace.t = 1;
			//_CollisionMap(qbsp, pEntity);
			_CollisionEntities(qbsp, pEntity);

			if(pEntity->m_gfxTrace.t < 1)
			{
				vel = pEntity->GetVel();
				mrT = pEntity->m_gfxTrace.t;
			}
			else
				pEntity->m_gfxTrace = traceTmp;
		}
#else
		if(!pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
		{
			gfxTrace traceTmp=pEntity->m_gfxTrace;

			pEntity->SetVel(norm*0.01f);
			pEntity->m_gfxTrace.t = 1;
			_CollisionMap(qbsp, pEntity);
			//_CollisionEntities(qbsp, pEntity);

			ptInOut += (pEntity->m_gfxTrace.t*0.01f)*norm;

			/*if(pEntity->m_gfxTrace.t < 1)
			{
				vel = pEntity->GetVel();
				mrT = pEntity->m_gfxTrace.t;
			}
			else*/
				pEntity->m_gfxTrace = traceTmp;
		}
#endif

		//get new point from the ray equation with 't'
		ptInOut += vel*mrT;

		////////////////////////////////////
		// check for stairs
		float ofsY = pEntity->BoundGetRadius();//2;

		D3DXVECTOR3 stairLoc(ptInOut.x, ptInOut.y+ofsY, ptInOut.z);
		D3DXVECTOR3 stairVel(vel.x, 0, vel.z), stairVel2(0, -ofsY, 0);
		D3DXVECTOR3 stairLocNext(ptInOut.x+vel.x, ptInOut.y+ofsY, ptInOut.z+vel.z);

		// make sure we are solid
		if(!pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
		{
			D3DXVECTOR3 tmpLoc = pEntity->GetLoc();
			D3DXVECTOR3 tmpVel = pEntity->GetVel();
			gfxTrace traceTmp=pEntity->m_gfxTrace;

			//check forward
			pEntity->m_gfxTrace.t = 1;

			pEntity->SetLoc(stairLoc);
			pEntity->SetVel(stairVel);

			_CollisionMap(qbsp, pEntity);

			//if we don't collide, check downward
			if(pEntity->m_gfxTrace.t >= 1)
			{
				pEntity->SetLoc(stairLocNext);
				pEntity->SetVel(stairVel2);

				_CollisionMap(qbsp, pEntity);
				
				//FIXME: better stairs check
				if(pEntity->m_gfxTrace.t < 1
					&& pEntity->m_gfxTrace.norm[eX] == 0
					&& pEntity->m_gfxTrace.norm[eZ] == 0)
				{
					ptInOut.x = stairLocNext.x;
					ptInOut.y = stairLocNext.y + pEntity->m_gfxTrace.t*(-ofsY);
					ptInOut.z = stairLocNext.z;

					//consider whatever action for walking
					pEntity->MoveAnim(MOVE_WALK);

					//we don't want to collide this guy anymore

					pEntity->SetLoc(ptInOut);

					pEntity->m_vel = g_zeroVel;

					goto DONE;
				}
				else
				{
					pEntity->SetLoc(tmpLoc);
					pEntity->SetVel(tmpVel);
					pEntity->m_gfxTrace = traceTmp;
				}
			}
			else
			{
				pEntity->SetLoc(tmpLoc);
				pEntity->SetVel(tmpVel);
				pEntity->m_gfxTrace = traceTmp;
			}
		}
		////////////////////////////////////

		//do something if we are colliding with world
		//if(!pEntity->m_bcollideWith)
			//pEntity->CollideWorld(qbsp, pEntity->m_gfxTrace);

		pEntity->SetLoc(ptInOut);

		////////////////////////////////////
		// Move with new slide velocity
		if(newVel.x != 0 || newVel.y != 0 || newVel.z != 0)
		{
			pEntity->m_vel = newVel;
		}
		else
		{
			//consider whatever action for wall or ground
			if(vel.y < 0)
				pEntity->MoveAnim(MOVE_NONE);

			//we don't want to collide this guy anymore
			pEntity->m_vel = g_zeroVel;

			goto DONE;
		}
		////////////////////////////////////
	}
	//didn't collide with anything!
	else if(!pEntity->m_gfxTrace.bStartSolid && !pEntity->m_gfxTrace.bAllSolid)
	{
		pEntity->SetLoc(pEntity->GetLoc()+pEntity->GetVel());

		//check to see if we want to fall or not...
		if(pEntity->GetVel().y != 0)
			pEntity->MoveAnim(MOVE_FALL);
		else
			pEntity->MoveAnim(MOVE_WALK);

		//we don't want to collide this guy anymore
		pEntity->m_vel = g_zeroVel;
		
		goto DONE;
	}

DONE:
	
	//pEntity->m_gfxTrace.t = 1;

	return;
}

//...it's pretty brutal, I say.
void _BrutalCollision(hQBSP qbsp, list< Id > & group)
{
	EntityCommon *pEntity1, *pEntity2;

	for(list<Id>::iterator i = group.begin(); i != group.end();)
	{
		list<Id>::iterator iNext = i; iNext++;

		pEntity1 = (EntityCommon *)IDPageQuery(*i);

		if(pEntity1)
		{
			for(list<Id>::iterator j = iNext; j != group.end(); j++)
			{
				pEntity2 = (EntityCommon *)IDPageQuery(*j);

				if(pEntity2)
				{
					pEntity1->CollAddEntity(*j);
					pEntity2->CollAddEntity(*i);
				}
			}
		}

		i = iNext;
	}
}

#define BOUND_TOLER		1.0f

PRIVATE void _ListOpenCloseBounds(EntityBoundDat *pBoundDatBuff, 
								  list< Id > & group, eAxis axis)
{
	EntityCommon *pEntity;

	//EntityBoundDat * pNewBoundDat = new EntityBoundDat[group.size()*2];
	EntityBoundDat *pBoundDatWalker = pBoundDatBuff;

	for(list<Id>::iterator i = group.begin(); i != group.end(); i++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*i);

		/*float ofs[eMaxPt] = {GAME_ABS(pEntity->GetVel().x),
			GAME_ABS(pEntity->GetVel().y),
			GAME_ABS(pEntity->GetVel().z)};


		if(pEntity->BoundGetMax() != pEntity->BoundGetMin()
			&& pEntity->BoundGetMax().x > ofs[eX]
			&& pEntity->BoundGetMax().y > ofs[eY]
			&& pEntity->BoundGetMax().z > ofs[eZ]
			&& !pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
		{
			ofs[eX] = 0;
			ofs[eY] = 0;
			ofs[eZ] = 0;
		}*/

		if(pEntity)
		{
			pBoundDatWalker->ID    = *i;
			pBoundDatWalker->bOpen = true;

			switch(axis)
			{
			case AXIS_X:
				pBoundDatWalker->val = pEntity->BoundGetMin().x + pEntity->GetLoc().x - BOUND_TOLER;

				if(pEntity->GetVel().x < 0)
					pBoundDatWalker->val += pEntity->GetVel().x;
				break;

			case AXIS_Y:
				pBoundDatWalker->val = pEntity->BoundGetMin().y + pEntity->GetLoc().y - BOUND_TOLER;

				if(pEntity->GetVel().y < 0)
					pBoundDatWalker->val += pEntity->GetVel().y;
				break;

			case AXIS_Z:
				pBoundDatWalker->val = pEntity->BoundGetMin().z + pEntity->GetLoc().z - BOUND_TOLER;

				if(pEntity->GetVel().z < 0)
					pBoundDatWalker->val += pEntity->GetVel().z;
				break;
			}

			pBoundDatWalker++;

			pBoundDatWalker->ID    = *i;
			pBoundDatWalker->bOpen = false;

			switch(axis)
			{
			case AXIS_X:
				pBoundDatWalker->val = pEntity->BoundGetMax().x + pEntity->GetLoc().x  + BOUND_TOLER;

				if(pEntity->GetVel().x > 0)
					pBoundDatWalker->val += pEntity->GetVel().x;
				break;

			case AXIS_Y:
				pBoundDatWalker->val = pEntity->BoundGetMax().y + pEntity->GetLoc().y  + BOUND_TOLER;

				if(pEntity->GetVel().y > 0)
					pBoundDatWalker->val += pEntity->GetVel().y;
				break;

			case AXIS_Z:
				pBoundDatWalker->val = pEntity->BoundGetMax().z + pEntity->GetLoc().z  + BOUND_TOLER;

				if(pEntity->GetVel().z > 0)
					pBoundDatWalker->val += pEntity->GetVel().z;
				break;
			}

			pBoundDatWalker++;
		}
		else
		{
			pBoundDatWalker->ID.counter = -1;
			pBoundDatWalker++;
			pBoundDatWalker->ID.counter = -1;
			pBoundDatWalker++;
		}
	}
}

int _BoundDatCompare( const void *arg1, const void *arg2 )
{
	EntityBoundDat *b1 = (EntityBoundDat *)arg1;
	EntityBoundDat *b2 = (EntityBoundDat *)arg2;

	if(b1->val < b2->val)
		return -1;
	else if(!GAME_IDVALID(b1->ID) || b1->val > b2->val)
		return 1;

	return 0;
}


//this is the Recursive Dimensional Clustering thing-a-magig fancy snancy
//ultra fast super hyper accurate awesome collision.
void RDC(hQBSP qbsp, list< Id > & group, 
		 eAxis axis1, eAxis axis2, eAxis axis3,
		 int boundStartInd = 0)
{
	//uh oh...
	if(boundStartInd >= g_entityBoundsSize)
	{ assert(0); return; }

	//check to see if the group is small enough for brute force
	if(group.size() < MAX_NUM_BRUTE_FORCE || axis1 == AXIS_INVALID)
	{
		_BrutalCollision(qbsp, group);
		return;
	}

	int boundSize = group.size()*2;

	//uh oh...
	if(boundStartInd+boundSize > g_entityBoundsSize)
	{ assert(0); return; }

	EntityBoundDat *pBoundDatBuff = &g_entityBounds[boundStartInd];

	//create the buffer
	_ListOpenCloseBounds(pBoundDatBuff, group, axis1);

	//now sort it
	qsort(pBoundDatBuff, boundSize, sizeof(EntityBoundDat), _BoundDatCompare);

	//////////////////////////////////////////////
	//let's slice 'n dice 'em
	list< Id > subGroup;

	int count = 0;

	eAxis newAxis1 = axis2;
	eAxis newAxis2 = axis3;
	
	bool bGroupSubdivided = false;

	for(int i = 0; i < boundSize; i++)
	{
		//Put this in the group
		if(pBoundDatBuff[i].bOpen && GAME_IDVALID(pBoundDatBuff[i].ID))
		{
			count++;

			subGroup.push_back(pBoundDatBuff[i].ID);
		}
		else
		{
			count--;

			//this means that we've grouped all stuff in the axis
			if(count == 0)
			{
				//now we will divide this group by recursing.

				//check to see if we want to furthermore subdivide
				if(i != boundSize-1)
					bGroupSubdivided = true;

				//set new axis'
				if(bGroupSubdivided)
				{
					switch(axis1)
					{
					case AXIS_X:
						newAxis1 = AXIS_Y;
						newAxis2 = AXIS_Z;
						break;

					case AXIS_Y:
						newAxis1 = AXIS_X;
						newAxis2 = AXIS_Z;
						break;

					case AXIS_Z:
						newAxis1 = AXIS_X;
						newAxis2 = AXIS_Y;
						break;
					}
				}

				RDC(qbsp, subGroup, newAxis1, newAxis2, AXIS_INVALID,
					boundStartInd+boundSize);

				subGroup.clear();
			}
		}
	}
}

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
PUBLIC void CollisionExecute(hQBSP qbsp)
{
	int numDoneCollide = 0;

	EntityCommon *pEntity;

	list<Id>::iterator it;

	//////////////////////////////////////////////////////////////
	//move the doodads
	for(it = g_doodadList.begin(); it != g_doodadList.end(); it++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			pEntity->SetLoc(pEntity->GetLoc()+pEntity->GetVel());
			pEntity->SetVel(g_zeroVel);
		}
	}
	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	//collide entities with each other, including world
	RDC(qbsp, g_entityList, AXIS_X, AXIS_Y, AXIS_Z);
	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	//execute collision
	for(it = g_entityList.begin(); it != g_entityList.end(); it++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*it);

		if(pEntity)
		{
			pEntity->m_tShadow = 1;

			pEntity->m_LastCollided.counter = -1;

			for(int ack = 0; ack < 2; ack++)
			{
				//collide to world
				memset(&pEntity->m_gfxTrace, 0, sizeof(pEntity->m_gfxTrace));
				pEntity->m_gfxTrace.t = 1;
				pEntity->m_bcollideWith = false;

				_CollisionMap(qbsp, pEntity);

				if(pEntity->m_gfxTrace.t < 1 
					&& pEntity->m_gfxTrace.norm[eY] > 0
					&& pEntity->m_vel.y != 0)
				{
					pEntity->m_tShadow = 0;
					pEntity->m_shadowLoc = pEntity->GetLoc();
					pEntity->m_shadowLoc.y -= pEntity->BoundGetMax().y + SHADOW_OFS;
				}

				_CollisionSet(qbsp, pEntity);

				//check to see if we get stuck...
				//both must be solid
				/*EntityCommon *pColEnt = (EntityCommon *)IDPageQuery(pEntity->m_LastCollided);
				if(pColEnt 
					&& !pColEnt->CheckFlag(ENTITY_FLAG_NONSOLID)
					&& !pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
				{
					Bound *pBound = dynamic_cast<Bound*>(pColEnt);
					if(pEntity->BoundBoxCheck(pEntity->GetLoc(), *pBound, pColEnt->GetLoc()))
					{
						int asshole = 0;
					}
				}*/

				if(pEntity->GetVel() == g_zeroVel)
				{ pEntity->m_bPushed = false; break; }
				else
					pEntity->m_bPushed = true;
			}

			if(pEntity->m_tShadow == 1)
				_CollisionMapShadow(qbsp, pEntity);

			pEntity->CollClear();
		}
	}
	//////////////////////////////////////////////////////////////
}

PRIVATE inline void _CollisionPrep(hCFG profile, list< Id > entities)
{
	EntityCommon *pEntity;

	for(list<Id>::iterator i = entities.begin(); i != entities.end(); i++)
	{
		pEntity = (EntityCommon *)IDPageQuery(*i);

		if(pEntity)
			pEntity->ProfileCheckReq(profile);
	}
}

/////////////////////////////////////
// Name:	CollisionPrepare
// Purpose:	prepare the entities based
//			on profile
//			NOTE: assumes that stages
//			has been loaded
// Output:	enable/disable doodads,
//			activate/deactivate entities
// Return:	none
/////////////////////////////////////
PUBLIC void CollisionPrepare(hCFG profile)
{
	_CollisionPrep(profile, g_doodadList);
	_CollisionPrep(profile, g_entityList);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CollisionBltShadows()
{
	if(g_world)
	{
		GFXEnableLight(false);

		EntityCommon *pEntity;

		for(list<Id>::iterator it = g_entityList.begin(); it != g_entityList.end(); it++)
		{
			pEntity = (EntityCommon *)IDPageQuery(*it);

			if(pEntity)
			{
				if(pEntity->m_tShadow < 1)
				{
					float norm[eMaxPt]={0,1,0};

					TextureBltDecal(g_shadowTxt, (float*)pEntity->m_shadowLoc,
								   pEntity->BoundGetRadius(), norm, 0, 0, 0);
				}
			}
		}

		GFXEnableLight(true);
	}
}