#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

Whip::Whip(const Id & owner, int whipEndInd, float lengthMax, float spd, double stunDelay, int grappleState)
: Projectile(PROJ_WHIP, owner), m_whipEndInd(whipEndInd), m_normJoint(0,0,1), 
m_lengthAccum(0), m_lengthMax(lengthMax), m_stunDelay(stunDelay), 
m_grappleState(grappleState)
{
	SetFlag(PROJ_FLAG_OBJINVUL, true);

	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);
	hOBJ obj = pEntity->GetOBJ();

	if(pEntity && obj)
	{
		//set speed
		m_spd.MoveSetSpeed(spd);

		//set direction
		SetDir(pEntity->GetDir());

		D3DXVECTOR3 v(1, 1, 1);

		BoundSetMax(v);
		BoundSetMin(-v);
		BoundSetRadius(1);
	}
}

Whip::~Whip()
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

	if(pEntity)
	{
		hOBJ obj = pEntity->GetOBJ();

		if(obj)
		{
			//set whip to normal
			OBJJointTranslate(obj, m_whipEndInd, 
				(float*)D3DXVECTOR3(0,0,0), false);
		}
	}
}

int Whip::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());
			hOBJ obj = pEntity->GetOBJ();

			if(pEntity && obj)
			{
				float spd = m_spd.MoveUpdate(g_timeElapse);

				//we are currently grappling
				if(OBJGetCurState(obj) == m_grappleState)
				{
					m_lengthAccum -= spd;

					////////////////////////////////////////////////////
					//shrink the joint
					D3DXVECTOR3 v = -spd*m_normJoint;
					OBJJointTranslate(obj, m_whipEndInd, (float*)v, true);

					////////////////////////////////////////////////////
					//move owner
					//pEntity->SetLoc(pEntity->GetLoc());
					pEntity->SetVel(pEntity->GetDir()*spd);

					//done grappling?
					if(m_lengthAccum <= 0)
					{
						pEntity->SetVel(D3DXVECTOR3(0,0,0));
						//Poll ourself to death!
						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
				}
				else
				{
					//update length
					m_lengthAccum += spd;

					////////////////////////////////////////////////////
					//stretch the joint
					D3DXVECTOR3 v = spd*m_normJoint;
					OBJJointTranslate(obj, m_whipEndInd, (float*)v, true);

					////////////////////////////////////////////////////
					//set location
					OBJJointGetWorldLoc(obj, m_whipEndInd, (float*)v);

					D3DXVECTOR3 loc(pEntity->GetLoc());
					loc.y = v.y;
					
					SetLoc(loc);

					////////////////////////////////////////////////////
					//update velocity
					SetVel(v-loc);

					////////////////////////////////////////////////////
					//check to see if we reach the limit
					if(m_lengthAccum >= m_lengthMax)
						//Poll ourself to death!
						SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			int sndInd = -1;

			//Did we hit a grappling object?
			if(pEntity && pOwner)
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
				&& pEntity->GetSubType() == OBJECT_STEAK)
				{
					EntityCommon *pEntOwner = (EntityCommon *)IDPageQuery(GetOwner());
					hOBJ obj = pEntOwner->GetOBJ();

					//start grappling if we haven't yet.
					if(pEntOwner 
						&& obj
						&& OBJGetCurState(obj) != m_grappleState)
					{
						OBJSetState(obj, m_grappleState);

						pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_STAKE, (LPARAM)&sndInd);

						//play sound
						if(sndInd > -1)
						{
							BASS_3DVECTOR pos, orient;
							memcpy(&pos, (float*)pEntity->GetLoc(), sizeof(pos)); pos.z *= -1;
							memcpy(&orient, (float*)pEntity->GetDir(), sizeof(orient)); orient.z *= -1;

							TaTaSoundPlay(sndInd, &pos, &orient, 0);
						}
					}
				}
				//check to see if the owner is not the same type as entity
				else if(pOwner->GetEntityType() != pEntity->GetEntityType())
				{
					//check to see if the entity is a creature
					if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
						|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
					{
						//hit this fella
						Creature *pCre = (Creature *)pEntity;

						pCre->Stun(m_stunDelay);

						pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_CRE, (LPARAM)&sndInd);

						//play sound
						if(sndInd > -1)
						{
							BASS_3DVECTOR pos, orient;
							memcpy(&pos, (float*)pCre->GetLoc(), sizeof(pos)); pos.z *= -1;
							memcpy(&orient, (float*)pCre->GetDir(), sizeof(orient)); orient.z *= -1;

							TaTaSoundPlay(sndInd, &pos, &orient, 0);
						}
					}

					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}
				else
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
			}
		}
		break;

	case ENTITYMSG_WORLDCOLLIDE: //(WPARAM)hQBSP, (LPARAM)txtID
		{
			//stop the attack
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity)
			{
				//also check if we are grappling
				//make sure we are not grappling
				if(OBJGetCurState(pEntity->GetOBJ()) != m_grappleState)
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
			}
		}
		break;

	case ENTITYMSG_DEATH:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}