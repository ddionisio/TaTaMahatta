#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

#define GEYSER_GAS_SPAWN_DELAY		100
#define GEYSER_GAS_MAX_PARTICLE		150
#define GEYSER_GAS_SIZE				10
#define GEYSER_GAS_DELAY_SCALE_MIN	250
#define GEYSER_GAS_DELAY_SCALE_MAX	500
#define GEYSER_GAS_DELAY_ALPHA_MIN	500
#define GEYSER_GAS_DELAY_ALPHA_MAX	1000

Geyser::Geyser() : Object(OBJECT_GEYSER), m_particle(0), m_bAlwaysActive(true),
m_bInActive(false)
{
	//we do not want geysers to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);
}

Geyser::~Geyser()
{
	if(m_particle)
		PARFXDestroy(&m_particle);
}

void Geyser::GeyserSetAlwaysActive(bool bYes)
{
	m_bAlwaysActive = bYes;

	m_bInActive = false;
}

void Geyser::GeyserSetActivateDelay(double delay)
{
	if(delay < 0)
		GeyserSetAlwaysActive(true);
	else
	{
		TimeInit(&m_activeDelay, delay);
		GeyserSetAlwaysActive(false);
	}
}

void Geyser::GeyserSetInActivateDelay(double delay)
{
	TimeInit(&m_deactiveDelay, delay);
}

void Geyser::GeySerSetUpSpd(float amt)
{
	m_upSpd.MoveSetSpeed(amt);
}

void Geyser::GeyserInitParticle(hTXT gasImg, BYTE r, BYTE g, BYTE b)
{
	if(m_particle)
		PARFXDestroy(&m_particle);

	if(gasImg)
	{
		fxGas_init tFX;

		tFX.gasTxt = gasImg;

		tFX.r = r;
		tFX.g = g;
		tFX.b = b;

		tFX.dir[eX] = tFX.dir[eZ] = 0;
		tFX.dir[eY] = 1;

		tFX.spd = m_upSpd.MoveGetSpeed()/2;

		tFX.spawnDelay = GEYSER_GAS_SPAWN_DELAY;

		memcpy(tFX.min, (float*)(BoundGetMin()+GetLoc()), sizeof(float)*eMaxPt);
		memcpy(tFX.max, (float*)(BoundGetMax()+GetLoc()), sizeof(float)*eMaxPt);

		tFX.maxParticle = GEYSER_GAS_MAX_PARTICLE;
		
		tFX.scaleMax = GEYSER_GAS_SIZE;

		tFX.scaleDelayMin = GEYSER_GAS_DELAY_SCALE_MIN;
		tFX.scaleDelayMax = GEYSER_GAS_DELAY_SCALE_MAX;

		tFX.alphaDelayMin = GEYSER_GAS_DELAY_ALPHA_MIN;
		tFX.alphaDelayMax = GEYSER_GAS_DELAY_ALPHA_MAX;

		tFX.bRepeat = true;

		m_particle = PARFXCreate(ePARFX_GAS, &tFX, -1, 0, -1, 0);
	}
}

int Geyser::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			EntityCommon *pEntity;

			D3DXVECTOR3 v;
			float spd = m_upSpd.MoveUpdate(g_timeElapse);

			for(list<Id>::iterator i = m_entities.begin(); i != m_entities.end(); i++)
			{
				pEntity = (EntityCommon*)IDPageQuery(*i);

				if(pEntity)
				{
					v = pEntity->GetVel();
					v.y = spd;
					pEntity->SetVel(v);
				}
			}

			m_entities.clear();

			if(!m_bAlwaysActive)
			{
				if(m_bInActive)
				{
					if(TimeElapse(&m_deactiveDelay))
					{
						m_bInActive = false;
						TimeReset(&m_activeDelay);

						if(m_particle)
							PARFXActivate(m_particle, true);
					}
				}
				else
				{
					if(TimeElapse(&m_activeDelay))
					{
						m_bInActive = true;
						TimeReset(&m_deactiveDelay);

						if(m_particle)
							PARFXActivate(m_particle, false);
					}
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			if(!m_bInActive)
			{
				EntityCommon *pEntity = (EntityCommon *)wParam;

				//check to see if already in list
				bool bAdd = true;

				for(list<Id>::iterator i = m_entities.begin(); i != m_entities.end(); i++)
				{
					if(GAME_IDISEQUAL(*i, pEntity->GetID()))
					{
						bAdd = false;
						break;
					}
				}

				if(bAdd)
					m_entities.push_back(pEntity->GetID());
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}
