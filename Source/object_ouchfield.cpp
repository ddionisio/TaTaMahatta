#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

#define OUCH_GAS_SPEED				5
#define OUCH_GAS_SPAWN_DELAY		100
#define OUCH_GAS_MAX_PARTICLE		150
#define OUCH_GAS_SIZE				20
#define OUCH_GAS_DELAY_SCALE_MIN	500
#define OUCH_GAS_DELAY_SCALE_MAX	1000
#define OUCH_GAS_DELAY_ALPHA_MIN	500
#define OUCH_GAS_DELAY_ALPHA_MAX	1000

OuchField::OuchField() : Object(OBJECT_OUCHFIELD), m_hitAmt(0), m_bGas(false)
{
	//we do not want ouch fields to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);
}

OuchField::~OuchField()
{
}

void OuchField::OuchSetHitAmt(int hit)
{
	m_hitAmt = hit;
}

void OuchField::OuchSetGasFlag(bool bYes)
{
	m_bGas = bYes;
}

void OuchField::OuchSetFX(hTXT gasImg, BYTE r, BYTE g, BYTE b)
{
	fxGas_init tFX;

	tFX.gasTxt = gasImg;

	tFX.r = r;
	tFX.g = g;
	tFX.b = b;

	tFX.dir[eX] = tFX.dir[eZ] = 0;
	tFX.dir[eY] = 1;

	tFX.spd = OUCH_GAS_SPEED;

	tFX.spawnDelay = OUCH_GAS_SPAWN_DELAY;

	memcpy(tFX.min, (float*)(BoundGetMin()+GetLoc()), sizeof(float)*eMaxPt);
	memcpy(tFX.max, (float*)(BoundGetMax()+GetLoc()), sizeof(float)*eMaxPt);

	tFX.maxParticle = OUCH_GAS_MAX_PARTICLE;
	
	tFX.scaleMax = OUCH_GAS_SIZE;

	tFX.scaleDelayMin = OUCH_GAS_DELAY_SCALE_MIN;
	tFX.scaleDelayMax = OUCH_GAS_DELAY_SCALE_MAX;

	tFX.alphaDelayMin = OUCH_GAS_DELAY_ALPHA_MIN;
	tFX.alphaDelayMax = OUCH_GAS_DELAY_ALPHA_MAX;

	tFX.bRepeat = true;

	PARFXCreate(ePARFX_GAS, &tFX, -1, 0, -1, 0);
}

int OuchField::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(m_bGas)
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA 
					&& !pEntity->CheckFlag(CRE_FLAG_GASIMMUNE))
				{
					Creature *pCre = (Creature *)pEntity;
					pCre->Hit(m_hitAmt);
				}
			}
			else
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
					|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
				{
					Creature *pCre = (Creature *)pEntity;
					pCre->Hit(m_hitAmt);
				}
				else if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
					&& pEntity->GetSubType() == OBJECT_BLOCK)
					pEntity->SetFlag(ENTITY_FLAG_POLLDEATH, true);
			}
		}
		break;

	case ENTITYMSG_ENTITYNOCOLLIDE:
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}