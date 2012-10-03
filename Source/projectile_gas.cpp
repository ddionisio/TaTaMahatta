#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

/*
class Gasus : public Projectile {
public:
	Gasus(const Id & owner, const D3DXVECTOR3 & bound, 
		const D3DXVECTOR3 & gasDir, hTXT gasTxt, float gasSize, 
		BYTE r, BYTE g, BYTE b, float duration);

	~Gasus();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	win32Time	m_duration;			//duration of gas
	bool		m_bInfinite;		//no duration?
	hPARFX		m_particle;			//the particle handle
};*/
#define GASUS_FX_SPEED				5
#define GASUS_FX_SPAWN_DELAY		50
#define GASUS_FX_MAX_PARTICLE		200
#define GASUS_FX_DELAY_SCALE_MIN	500
#define GASUS_FX_DELAY_SCALE_MAX	1000
#define GASUS_FX_DELAY_ALPHA_MIN	500
#define GASUS_FX_DELAY_ALPHA_MAX	1000

Gasus::Gasus(const Id & owner, const D3DXVECTOR3 & loc,
		const D3DXVECTOR3 & bound, const D3DXVECTOR3 & gasDir, 
		hTXT gasTxt, float gasSize, BYTE r, BYTE g, BYTE b, float duration) 
: Projectile(PROJ_GAS, owner), m_bInfinite(false), m_particle(0)
{
	SetFlag(PROJ_FLAG_OBJINVUL, true);
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);
	hOBJ obj = 0;

	if(pEntity)
	{
		obj = pEntity->GetOBJ();

		////////////////////////////////////////////////////////
		//create the bound
		BoundSetMin(-bound);
		BoundSetMax(bound);

		float len = D3DXVec3Length(&bound);
		BoundSetRadius(len);
	}

	SetLoc(loc);

	//set duration if available
	if(duration < 0)
		m_bInfinite = true;
	else
		TimeInit(&m_duration, duration);

	//create the gas FX
	if(gasTxt)
	{
		fxGas_init tFX;

		tFX.gasTxt = gasTxt;

		tFX.r = r;
		tFX.g = g;
		tFX.b = b;

		tFX.dir[eX] = gasDir.x;
		tFX.dir[eY] = gasDir.y;
		tFX.dir[eZ] = gasDir.z;

		tFX.spd = GASUS_FX_SPEED;

		tFX.spawnDelay = GASUS_FX_SPAWN_DELAY;

		memcpy(tFX.min, (float*)(BoundGetMin()), sizeof(float)*eMaxPt);
		memcpy(tFX.max, (float*)(BoundGetMax()), sizeof(float)*eMaxPt);

		tFX.maxParticle = GASUS_FX_MAX_PARTICLE;
		
		tFX.scaleMax = gasSize;

		tFX.scaleDelayMin = GASUS_FX_DELAY_SCALE_MIN;
		tFX.scaleDelayMax = GASUS_FX_DELAY_SCALE_MAX;

		tFX.alphaDelayMin = GASUS_FX_DELAY_ALPHA_MIN;
		tFX.alphaDelayMax = GASUS_FX_DELAY_ALPHA_MAX;

		tFX.bRepeat = true;

		m_particle = PARFXCreate(ePARFX_GAS, &tFX, -1, obj, -1, 0);
	}
}

Gasus::~Gasus()
{
	if(m_particle)
		PARFXDestroy(&m_particle);
}

int Gasus::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			if(!m_bInfinite)
			{
				if(TimeElapse(&m_duration))
				{
					//Poll ourself to death!
					SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}
			}

			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			if(pOwner)
				SetLoc(pOwner->GetLoc());
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			//check to see if the owner is not the same type as entity
			if(pOwner && pOwner->GetEntityType() != pEntity->GetEntityType())
			{
				//check to see if the entity is a creature
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
					|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
				{
					//OUCH!
					Creature *pCre = (Creature *)pEntity;

					pCre->Hit();
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