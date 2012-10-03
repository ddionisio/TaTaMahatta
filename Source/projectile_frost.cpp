#include "tata_main.h"

#include "tata_globals.h"

#include "tata_projectiles_common.h"

/*
class Frost : public Projectile {
public:
	Frost(const Id & owner, D3DXVECTOR3 bound, int jBInd, int jEInd, hTXT frostTxt, BYTE r, BYTE g, BYTE b);

	~Frost();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int		m_jBInd, m_jEInd;	//joint index begin and end
	hTXT	m_frostTxt;			//particle texture
	BYTE	m_r,m_g,m_b;		//particle color
};*/

Frost::Frost(const Id & owner, const D3DXVECTOR3 & bound, int jBInd, int jEInd, 
		hTXT frostFXTxt, hMDL frozenMdl, hTXT frozenTxt, BYTE r, BYTE g, BYTE b)
: Projectile(PROJ_FROST, owner), m_jBInd(jBInd), m_jEInd(jEInd), m_r(r), m_g(g), m_b(b), m_particle(0)
{
	EntityCommon *pEntity = (EntityCommon *)IDPageQuery(owner);
	hOBJ obj = pEntity->GetOBJ();

	////////////////////////////////////////////////////////
	//set frost FX texture
	m_frostFXTxt = frostFXTxt; TextureAddRef(m_frostFXTxt);

	////////////////////////////////////////////////////////
	//set frozen model chunk
	m_frozenMdl = frozenMdl; MDLAddRef(m_frozenMdl);

	////////////////////////////////////////////////////////
	//set frozen texture
	m_frozenTxt = frozenTxt; TextureAddRef(m_frozenTxt);

	////////////////////////////////////////////////////////
	//create particle FX
	fxSmoke_init parDat={0};

	parDat.smokeTxt		= m_frostFXTxt;
	parDat.r			= m_r;
	parDat.g			= m_g;
	parDat.b			= m_b;
	parDat.spawnDelay	= 100;
	parDat.spdDelay		= 1000;
	parDat.fadeDelay	= 250;

	parDat.size			= 5;
	
	parDat.min[eX] = -bound.x; parDat.min[eY] = -bound.y; parDat.min[eZ] = -bound.z;
	parDat.max[eX] = bound.x; parDat.max[eY] = bound.y; parDat.max[eZ] = bound.z;

	parDat.eJointInd	= m_jEInd;
	parDat.maxParticle	= 25;
	parDat.axis			= 1;
	parDat.bRepeat		= true;

	m_particle = PARFXCreate(ePARFX_SMOKE, &parDat, -1, obj, m_jBInd, 0);

	SetFlag(PROJ_FLAG_OBJINVUL, true);

	if(pEntity && obj)
	{
		////////////////////////////////////////////////////////
		//create the bound
		BoundSetMin(-bound);
		BoundSetMax(bound);

		float len = D3DXVec3Length(&bound);
		BoundSetRadius(len);
	}
}

Frost::~Frost()
{
	if(m_frostFXTxt)
		TextureDestroy(&m_frostFXTxt);

	if(m_frozenMdl)
		MDLDestroy(&m_frozenMdl);

	if(m_frozenTxt)
		TextureDestroy(&m_frozenTxt);

	if(m_particle)
		PARFXDestroy(&m_particle);
}

int Frost::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(GetOwner());

			if(pEntity)
			{
				hOBJ obj = pEntity->GetOBJ();

				if(obj)
				{
					D3DXVECTOR3 sLoc, eLoc;

					OBJJointGetWorldLoc(obj, m_jBInd, (float*)sLoc);
					OBJJointGetWorldLoc(obj, m_jEInd, (float*)eLoc);

					SetLoc(sLoc);
					SetVel(eLoc-sLoc);
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE://, (WPARAM)pEntity
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			EntityCommon *pOwner = (EntityCommon *)IDPageQuery(GetOwner());

			//check to see if the owner is not the same type as entity
			if(pOwner 
				&& (pOwner->GetEntityType() != pEntity->GetEntityType()))
			{
				//check to see if the entity is a creature
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
					|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
				{
					//freeze this fella
					Creature *pCre = (Creature *)pEntity;

					pCre->Freeze(m_frozenTxt, m_frozenMdl);
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