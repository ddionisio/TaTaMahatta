#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

//Attack Melee and Attack Melee 2

AttackMelee::AttackMelee(const Id & owner, const D3DXVECTOR3 & bound, int jointInd) 
: Projectile(PROJ_ATTACKMELEE, owner), m_jointInd(jointInd)
{
	////////////////////////////////////////////////////////
	//create the bound
	BoundSetMin(-bound);
	BoundSetMax(bound);

	float len = D3DXVec3Length(&bound);
	BoundSetRadius(len);
}

AttackMelee::~AttackMelee()
{
}

int AttackMelee::Callback(unsigned int msg, unsigned int wParam, int lParam)
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
				{
					if(m_jointInd != -1)
					{
						D3DXVECTOR3 cPt;

						//set location
						OBJJointGetWorldLoc(obj, m_jointInd, (float*)cPt);
						SetLoc(cPt);

						//set dir and velocity
						SetDir(pEntity->GetDir());
						SetVel(pEntity->GetDir());
					}
				}
			}
		}
		break;
	
	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			//play a sound if ind is > -1
			int sndInd = -1;

			if(pOwner)
			{
				//check to see if the owner is not the same type as entity
				//also make sure this entity is not immune
				if(pOwner->GetEntityType() != pEntity->GetEntityType()
					&& !pEntity->CheckFlag(CRE_FLAG_MELEEIMMUNE))
				{
					//check to see if the entity is a creature
					if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
						|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
					{
						//hit this fella
						Creature *pCre = (Creature *)pEntity;

						if(!pCre->Hit())
						{
							OBJSetState(pOwner->GetOBJ(), CREANIM_IDLE1);
							pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);
						}
						else
							pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_CRE, (LPARAM)&sndInd);

						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
					else
					{
						//Poll ourself to death!
						SetFlag(ENTITY_FLAG_POLLDEATH, true);
						OBJSetState(pOwner->GetOBJ(), CREANIM_IDLE1);

						pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);
					}
				}
				else
				{
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
					OBJSetState(pOwner->GetOBJ(), CREANIM_IDLE1);

					pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);
				}
			}

			//play sound
			if(sndInd > -1)
			{
				BASS_3DVECTOR pos, orient;
				memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
				memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

				TaTaSoundPlay(sndInd, &pos, &orient, 0);
			}
		}
		break;

	case ENTITYMSG_WORLDCOLLIDE: //(WPARAM)hQBSP, (LPARAM)txtID
		{
			//stop the attack and set the owner back to normal state
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity)
			{
				OBJSetState(pEntity->GetOBJ(), CREANIM_IDLE1);

				//Poll ourself to death!
				SetFlag(ENTITY_FLAG_POLLDEATH, true);

				int sndInd=-1;
				pEntity->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);

				//play sound
				if(sndInd > -1)
				{
					BASS_3DVECTOR pos, orient;
					memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
					memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

					TaTaSoundPlay(sndInd, &pos, &orient, 0);
				}
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

/////////////////////////////////////////////////////////////////////////////
// Attack Melee 2 (similar to AttackMelee, uses two end points rather than one)
// The given 'bound' must have positive values for x,y,z
/////////////////////////////////////////////////////////////////////////////
AttackMelee2::AttackMelee2(const Id & owner, const D3DXVECTOR3 & bound, int jointBegin, int jointEnd)
: Projectile(PROJ_ATTACKMELEE, owner), m_jBInd(jointBegin), m_jEInd(jointEnd)
{
	////////////////////////////////////////////////////////
	//create the bound
	BoundSetMin(-bound);
	BoundSetMax(bound);

	float len = D3DXVec3Length(&bound);
	BoundSetRadius(len);
}

AttackMelee2::~AttackMelee2()
{
}

int AttackMelee2::Callback(unsigned int msg, unsigned int wParam, int lParam)
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
				{
					//update location and velocity
					D3DXVECTOR3 beginLoc, endLoc;

					//They better have these two joints!!!
					if(m_jBInd != -1 && m_jEInd != -1)
					{
						//get the beginning loc
						OBJJointGetWorldLoc(obj, m_jBInd, (float*)beginLoc);

						//get the ending loc
						OBJJointGetWorldLoc(obj, m_jEInd, (float*)endLoc);

						//set origin
						SetLoc(beginLoc);

						//set velocity
						SetVel(endLoc - beginLoc);
					}
				}
			}
		}
		break;
	
	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			//play a sound if ind is > -1
			int sndInd = -1;

			if(pOwner)
			{
				//check to see if the owner is not the same type as entity
				//also make sure this entity is not immune
				if(pOwner->GetEntityType() != pEntity->GetEntityType()
					&& !pEntity->CheckFlag(CRE_FLAG_MELEEIMMUNE))
				{
					//check to see if the entity is a creature
					if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
						|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
					{
						//hit this fella
						Creature *pCre = (Creature *)pEntity;

						if(!pCre->Hit())
						{
							OBJSetState(pOwner->GetOBJ(), CREANIM_IDLE1);
							pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);
						}
						else
							pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_CRE, (LPARAM)&sndInd);

						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
					else
					{
						//Poll ourself to death!
						SetFlag(ENTITY_FLAG_POLLDEATH, true);
						OBJSetState(pOwner->GetOBJ(), CREANIM_IDLE1);

						pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);
					}
				}
				else
				{
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
					OBJSetState(pOwner->GetOBJ(), CREANIM_IDLE1);

					pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);
				}
			}

			//play sound
			if(sndInd > -1)
			{
				BASS_3DVECTOR pos, orient;
				memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
				memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

				TaTaSoundPlay(sndInd, &pos, &orient, 0);
			}
		}
		break;

	case ENTITYMSG_WORLDCOLLIDE: //(WPARAM)hQBSP, (LPARAM)txtID
		{
			//stop the attack and set the owner back to normal state
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity)
			{
				OBJSetState(pEntity->GetOBJ(), CREANIM_IDLE1);

				//Poll ourself to death!
				SetFlag(ENTITY_FLAG_POLLDEATH, true);

				int sndInd = -1;
				pEntity->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_WALL, (LPARAM)&sndInd);

				//play sound
				if(sndInd > -1)
				{
					BASS_3DVECTOR pos, orient;
					memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
					memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

					TaTaSoundPlay(sndInd, &pos, &orient, 0);
				}
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