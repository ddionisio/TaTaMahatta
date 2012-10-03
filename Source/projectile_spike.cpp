#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

Spike::Spike(const Id & owner, hMDL model, const D3DXVECTOR3 & loc, 
		float endY, float spd)
		: Projectile(PROJ_SPIKE, owner), m_spikeDir(1),
		m_spikeMaxHeight(endY)
{
	//set the object
	if(model)
		SetOBJ(OBJCreate(0, model, 0,0,0, eOBJ_STILL, 0));

	//set location
	m_spikeMinHeight = loc.y;
	SetLoc(loc);

	//set bound
	if(GetOBJ())
	{
		gfxBound bound; OBJGetBounds(GetOBJ(), &bound, true);
		BoundSetMin(D3DXVECTOR3(bound.min));
		BoundSetMax(D3DXVECTOR3(bound.max));
	}

	//set speed
	m_spikeSpd.MoveSetSpeed(spd);

	//set some flag
	SetFlag(PROJ_FLAG_OBJINVUL, true);
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	//play slash sound
	BASS_3DVECTOR pos; memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
	BASS_3DVECTOR orient={0,1,0};

	TaTaSoundPlay(32, &pos, &orient, 0);
}

Spike::~Spike()
{
}

int Spike::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			//move based on direction
			D3DXVECTOR3 loc = GetLoc();
			loc.y += m_spikeSpd.MoveUpdate(m_spikeDir*g_timeElapse);
			SetLoc(loc);

			//check to see if we want to go down or something
			if(loc.y >= m_spikeMaxHeight && m_spikeDir > 0)
				m_spikeDir = -1;
			else if(loc.y <= m_spikeMinHeight && m_spikeDir < 0)
				//death
				SetFlag(ENTITY_FLAG_POLLDEATH, true);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			//check to see if the owner is not the same type as entity
			//make sure the spike is also going up
			if(pOwner && m_spikeDir > 0)
			{
				if(pOwner->GetEntityType() != pEntity->GetEntityType())
				{
					//check to see if the entity is a creature
					if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
						|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
					{
						//hit this fella
						Creature *pCre = (Creature *)pEntity;

						if(pCre->Hit())
							//play 'hit' sound
							pCre->CREPlaySound(5);
					}
				}
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}