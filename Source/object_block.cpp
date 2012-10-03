#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_creature.h"

#include "tata_projectiles_common.h"

//Object Block Interface
Block::Block() : Object(OBJECT_BLOCK), m_debrisMdl(0)
{
	SetFlag(ENTITY_FLAG_PULLABLE, true);
}

Block::~Block()
{
	if(m_debrisMdl)
		MDLDestroy(&m_debrisMdl);
}

//set the debris model for this block
void Block::SetDebrisModel(hMDL mdl)
{
	if(m_debrisMdl)
		MDLDestroy(&m_debrisMdl);

	m_debrisMdl = mdl;
}

int Block::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			//get pushed by this projectile
			if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
			{
				Projectile *pProj = (Projectile *)pEntity;

				switch(pEntity->GetSubType())
				{
				case PROJ_PUSHMELEE:
					pEntity->AddPushList(GetID());
					break;

				case PROJ_ATTACKMELEE:
					{
						//destroy the projectile
						pEntity->SetFlag(ENTITY_FLAG_POLLDEATH, true);

						//make sure it belongs to Ta-Ta Trample
						Creature *pCre = (Creature *)IDPageQuery(pProj->GetOwner());

						if(pCre && pCre->GetEntityType() == ENTITY_TYPE_TATA
							&& pCre->GetSubType() == TATA_TATATRAMPLE)
						{
							//destroy ourself
							SetFlag(ENTITY_FLAG_POLLDEATH, true);
						}
					}
					break;
				}
			}
		}
		break;

	case ENTITYMSG_DEATH:
		/////////////////////////////////////////////////
		//create cool particle 
		if(m_debrisMdl)
		{
			fx3DExplode_init explode;

			explode.mdl = m_debrisMdl;
			explode.spd = 0.5f;
			
			explode.dir[eX] = explode.dir[eY] = explode.dir[eZ] = 0;

			explode.dirAcc[eX] = explode.dirAcc[eZ] = 0; explode.dirAcc[eY] = -0.025f;

			explode.min[eX] = explode.min[eY] = explode.min[eZ] = -50;
			explode.max[eX] = explode.max[eY] = explode.max[eZ] = 50;

			explode.maxParticle = 10;

			explode.delay = 10000;

			memcpy(explode.center, (float*)GetLoc(), sizeof(explode.center));

			PARFXCreate(ePARFX_3DEXPLODE, &explode, -1, 0, -1, ParticleCollisionCallback);
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		//if inactive
		if(CheckFlag(OBJ_FLAG_INACTIVE))
			return 0;
		break;
	}

	return RETCODE_SUCCESS;
}