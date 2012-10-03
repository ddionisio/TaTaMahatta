#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

UseMelee::UseMelee(const Id & owner, const D3DXVECTOR3 & bound, int jointBegin, int jointEnd) 
: Projectile(PROJ_USEMELEE, owner), m_jBInd(jointBegin), m_jEInd(jointEnd)
{
	//don't collide with world...
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	////////////////////////////////////////////////////////
	//create the bound
	BoundSetMin(-bound);
	BoundSetMax(bound);

	float len = D3DXVec3Length(&bound);
	BoundSetRadius(len);
}

UseMelee::~UseMelee()
{
}

int UseMelee::Callback(unsigned int msg, unsigned int wParam, int lParam)
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
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}