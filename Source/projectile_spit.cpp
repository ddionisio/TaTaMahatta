#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

_Spit::_Spit(const Id & owner, float spd, bool bBounce, int bounceMax, int sndInd)
: Projectile(PROJ_SPIT, owner), m_bBounce(bBounce), m_bounceMax(bounceMax),
m_bounceCur(0), m_tSnd(0)
{
	//set move
	m_spd.MoveSetSpeed(spd);

	SetFlag(PROJ_FLAG_OBJINVUL, bBounce);

	if(sndInd > -1)
	{
		BASS_3DVECTOR pos, orient;

		memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
		memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

		m_tSnd = TaTaSoundPlay(sndInd, &pos, &orient, 0);
	}
}

_Spit::~_Spit()
{
	if(m_tSnd)
		BASS_ChannelStop(m_tSnd);
}

int _Spit::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		//update velocity
		SetVel(GetDir()*m_spd.MoveUpdate(g_timeElapse));

		//update sound position
		if(m_tSnd)
		{
			BASS_3DVECTOR pos, orient, vel;

			memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
			memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;
			memcpy(&vel, (float*)GetVel(), sizeof(vel)); vel.z *= -1;

			BASS_ChannelSet3DPosition(m_tSnd, &pos, &orient, &vel);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			//check to see if the owner is not the same type as entity
			if(pOwner)
			{
				if(pOwner->GetEntityType() != pEntity->GetEntityType())
				{
					//check to see if the entity is a creature
					if((pEntity->GetEntityType() == ENTITY_TYPE_TATA
						|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
						&& !pEntity->CheckFlag(CRE_FLAG_SPITIMMUNE))
					{
						//hit this fella
						Creature *pCre = (Creature *)pEntity;

						if(pCre->Hit())
						{
							int sndInd = -1;
							pOwner->Callback(ENTITYMSG_REQUESTSOUND, SND_REQ_PROJ_HIT_CRE, (LPARAM)&sndInd);

							pCre->CREPlaySound(sndInd);
						}

						SetFlag(ENTITY_FLAG_POLLDEATH, true);
					}
				}
			}

			if(!m_bBounce || m_bounceCur == m_bounceMax)
				SetFlag(ENTITY_FLAG_POLLDEATH, true);
			else
			{
				//bounce off
				gfxTrace *pTrace = (gfxTrace*)lParam;

				D3DXVECTOR3 norm(pTrace->norm), refl;

				float nd = D3DXVec3Dot(&GetDir(), &norm);

				refl = GetDir() - (2*nd*norm);

				SetDir(refl);

				m_bounceCur++;
			}
		}
		break;

	case ENTITYMSG_WORLDCOLLIDE:
		{
			if(!m_bBounce || m_bounceCur == m_bounceMax)
				SetFlag(ENTITY_FLAG_POLLDEATH, true);
			else
			{
				//bounce off
				gfxTrace *pTrace = (gfxTrace*)lParam;

				D3DXVECTOR3 norm(pTrace->norm), refl;

				float nd = D3DXVec3Dot(&GetDir(), &norm);

				refl = GetDir() - (2*nd*norm);

				SetDir(refl);

				m_bounceCur++;
			}
		}
		break;

	case ENTITYMSG_DEATH:
		//cool FX explosion
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}


Spit::Spit(const Id & owner, D3DXVECTOR3 loc, float spd, hMDL spitMdl, hTXT trailTxt,
		   BYTE r, BYTE g, BYTE b, bool bBounce, int bounceMax, int sndInd)
: _Spit(owner, spd, bBounce, bounceMax, sndInd), m_r(r), m_g(g), m_b(b)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	if(pEntity)
	{
		//set the object
		SetOBJ(OBJCreate(0, spitMdl, 0,0,0, eOBJ_STILL, 0));

		//set location
		SetLoc(loc);

		//set direction
		SetDir(pEntity->GetDir());

		//set texture trail
		m_trailTxt = trailTxt;
		TextureAddRef(m_trailTxt);

		//set bound
		gfxBound bound;
		OBJGetBounds(GetOBJ(), &bound, true);
		BoundSetMin(D3DXVECTOR3(bound.min));
		BoundSetMax(D3DXVECTOR3(bound.max));
		BoundSetRadius(bound.radius);

		//create particle trail
		fxGas_init gasFX={0};
		gasFX.gasTxt		= m_trailTxt;
		gasFX.r				= m_r;
		gasFX.g				= m_g;
		gasFX.b				= m_b;
		gasFX.maxParticle	= 20;
		gasFX.scaleMax		= bound.radius;
		gasFX.scaleDelayMin = 250;
		gasFX.scaleDelayMax = 500;
		gasFX.alphaDelayMin = 250;
		gasFX.alphaDelayMax = 500;
		gasFX.bRepeat		= true;

		gasFX.spawnDelay	= 100;

		memcpy(gasFX.min, bound.min, sizeof(bound.min));
		memcpy(gasFX.max, bound.max, sizeof(bound.max));

		PARFXCreate(ePARFX_GAS, &gasFX, -1, GetOBJ(), -1, 0);
	}

	SetFlag(PROJ_FLAG_PUSHBUTTON, true);
}

Spit::~Spit()
{
	if(m_trailTxt)
		TextureDestroy(&m_trailTxt);
}

/*
class Spit2 : public _Spit {
public:
	Spit2(const Id & owner, D3DXVECTOR3 loc, float spd, hMDL spitMdl, hTXT EnergyTxt,
		float radius, BYTE r, BYTE g, BYTE b);

	~Spit2();

private:
	BYTE	m_r,m_g,m_b; //particle color

	hPARFX	energyFX;	//big ball of energy
};
*/

Spit2::Spit2(const Id & owner, D3DXVECTOR3 loc, float spd, hMDL spitMdl, hTXT EnergyTxt,
		float radius, BYTE r, BYTE g, BYTE b, bool bBounce, int bounceMax, int sndInd)
: _Spit(owner, spd, bBounce, bounceMax, sndInd)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);

	if(pEntity)
	{
		//set the object
		SetOBJ(OBJCreate(0, spitMdl, 0,0,0, eOBJ_STILL, 0));

		//set location
		SetLoc(loc);

		//set direction
		SetDir(pEntity->GetDir());

		//set bound
		D3DXVECTOR3 v(radius*0.75f, radius*0.75f, radius*0.75f);

		BoundSetMax(v);
		BoundSetMin(-v);
		BoundSetRadius(radius);

		//create particle trail
		fxPuffs_init tFX={0};
		tFX.puffTxt = EnergyTxt;
		tFX.r = r; tFX.g = g; tFX.b = b;
		tFX.maxParticle = 100;
		tFX.scaleStart = radius;
		tFX.scaleEnd = radius/4;
		tFX.delay = 800;

		m_energyFX = PARFXCreate(ePARFX_PUFFS, &tFX, -1, GetOBJ(), -1, 0);
	}

	SetFlag(PROJ_FLAG_PUSHBUTTON, true);
}

Spit2::~Spit2()
{
	if(m_energyFX)
		PARFXDestroy(&m_energyFX);
}
