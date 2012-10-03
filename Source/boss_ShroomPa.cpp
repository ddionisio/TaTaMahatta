#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	SHROOMPA_ANIM_IDLE,
	SHROOMPA_ANIM_IDLE2,
	SHROOMPA_ANIM_WALK,
	SHROOMPA_ANIM_CHARGE,
	SHROOMPA_ANIM_SPIT,
	SHROOMPA_ANIM_DIZZY,
	SHROOMPA_ANIM_OUCH,
	SHROOMPA_ANIM_DEATH
} eShroomPaAnim;

ShroomPa::ShroomPa() : EnemyCommon(BOSS_SHROOMPA), 
m_actionState(SHROOMPA_STATE_WANDER),
m_spitCur(0), m_chargeDir(0,0,0),
m_bDizzyAfterHit(false),
m_bGoToDest(false),
m_particle(0)
{
	for(int i = 0; i < SHROOMPA_STATE_MAX; i++)
		m_actionChance[i] = -1;

	SetFlag(CRE_FLAG_INVUL, true);
}

ShroomPa::~ShroomPa()
{
	if(m_particle)
		PARFXDestroy(&m_particle);

	if(m_spitMdl)
		MDLDestroy(&m_spitMdl);

	if(m_spitFXTxt)
		TextureDestroy(&m_spitFXTxt);
}

void ShroomPa::RandomChanceOfCrap()
{
	int r;

	for(int i = 0; i < SHROOMPA_STATE_MAX; i++)
	{
		r = rand()%100;

		if(m_actionChance[i] != -1
			&& r <= m_actionChance[i])
		{
			//gets re-enabled if the behavior desires it
			if(m_particle)
				PARFXActivate(m_particle, false);

			ChangeBehavior(i);

			break;
		}
	}
}

void ShroomPa::ChangeBehavior(int state)
{
	m_actionState = state;

	BehaviorUpdate(true);
}

void ShroomPa::BehaviorUpdate(bool bBegin)
{
	switch(m_actionState)
	{
	case SHROOMPA_STATE_WANDER:
		{
			if(bBegin)
			{
				if(m_particle)
					PARFXActivate(m_particle, true);

				OBJSetState(GetOBJ(), SHROOMPA_ANIM_WALK);
			}
			else if(TimeElapse(&m_actionDelay))
				//random chances of crap
				RandomChanceOfCrap();
		}
		break;

	case SHROOMPA_STATE_CHARGE:
		if(bBegin)
		{
			if(m_bGoToDest)
				ChangeBehavior(SHROOMPA_STATE_WANDER);
			else
			{
				SetMoveSpd(m_chargeSpd);

				//prepare charge
				OBJSetState(GetOBJ(), SHROOMPA_ANIM_IDLE);

				//roar sound
				CREPlaySound(26);

				TimeReset(&m_chargeDelay);

				//get old loc
				m_dest = GetLoc();
			}
		}
		else
		{
			SetDir(m_chargeDir);

			//ready to charge?
			if(OBJGetCurState(GetOBJ()) == SHROOMPA_ANIM_IDLE)
			{
				if(OBJIsStateEnd(GetOBJ()))
				{
					OBJSetState(GetOBJ(), SHROOMPA_ANIM_CHARGE);
				}
				else
				{
					//get current Ta-Ta and set the charge dir
					//m_chargeDir
					Creature *pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

					if(pCre)
						D3DXVec3Normalize(&m_chargeDir, &(pCre->GetLoc() - GetLoc()));
				}
			}
			//CHARGE!!!
			else if(OBJGetCurState(GetOBJ()) == SHROOMPA_ANIM_CHARGE)
			{
				MoveDir(1);

				if(TimeElapse(&m_chargeDelay))
				{
					m_bGoToDest = true;
					SetMoveSpd(m_normSpd);
					ChangeBehavior(SHROOMPA_STATE_WANDER);
				}
			}
		}

		break;

	case SHROOMPA_STATE_SPIT:
		if(bBegin)
		{
			//prepare charge
			OBJSetState(GetOBJ(), SHROOMPA_ANIM_SPIT);

			TimeReset(&m_chargeDelay);

			m_spitCur = 0;

			//roar 2 sound
			CREPlaySound(27);
		}
		else
		{
			SetDir(m_chargeDir);

			//ready to charge?
			if(OBJGetCurState(GetOBJ()) == SHROOMPA_ANIM_SPIT)
			{
				if(OBJIsStateEnd(GetOBJ()))
				{
					OBJSetState(GetOBJ(), SHROOMPA_ANIM_IDLE2);

					//set starting dir of spit
					D3DXMATRIX rMtx;
					D3DXMatrixRotationY(&rMtx, m_spitAngleStart);
					D3DXVec3TransformNormal(&m_chargeDir, &m_chargeDir, &rMtx);
					m_chargeDir.y = 0;
					D3DXVec3Normalize(&m_chargeDir, &m_chargeDir);
				}
				else
				{
					//get current Ta-Ta and set the charge dir
					//m_chargeDir
					Creature *pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

					if(pCre)
						D3DXVec3Normalize(&m_chargeDir, &(pCre->GetLoc() - GetLoc()));
				}
			}
			//SPIT!!!
			else if(OBJGetCurState(GetOBJ()) == SHROOMPA_ANIM_IDLE2)
			{
				if(OBJIsStateEnd(GetOBJ()))
					ChangeBehavior(SHROOMPA_STATE_WANDER);
				else if(m_spitCur < m_spitMax)
				{
					//set dir of spit
					D3DXMATRIX rMtx;
					D3DXMatrixRotationY(&rMtx, m_spitAngleInc);
					D3DXVec3TransformNormal(&m_chargeDir, &m_chargeDir, &rMtx);
					m_chargeDir.y = 0;
					D3DXVec3Normalize(&m_chargeDir, &m_chargeDir);

					//spit!
					//create spit projectile
					if(m_spitMdl)
					{
						D3DXVECTOR3 spitLoc;
						OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "mouth"), (float*)spitLoc);

						//cannon sound
						CREPlaySound(28);

						new Spit(GetID(), spitLoc, m_spitSpd, m_spitMdl, 
							m_spitFXTxt, m_r, m_g, m_b);
					}

					m_spitCur++;
				}
				else if(m_spitCur >= m_spitMax)
				{
					//get current Ta-Ta and set the charge dir
					//m_chargeDir
					Creature *pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

					if(pCre)
						D3DXVec3Normalize(&m_chargeDir, &(pCre->GetLoc() - GetLoc()));
				}
			}
		}
		break;

	case SHROOMPA_STATE_DIZZY:
		if(bBegin)
		{
			OBJSetState(GetOBJ(), SHROOMPA_ANIM_DIZZY);
			TimeReset(&m_dizzyDelay);
		}
		else if(m_bDizzyAfterHit)
		{
			if(TimeElapse(&m_dizzyAfterHitDelay))
			{
				m_bDizzyAfterHit = false;
				ChangeBehavior(SHROOMPA_STATE_WANDER);
				SetMoveSpd(m_normSpd);
			}
			else if(OBJIsStateEnd(GetOBJ())
				&& OBJGetCurState(GetOBJ()) != SHROOMPA_ANIM_DIZZY)
				OBJSetState(GetOBJ(), SHROOMPA_ANIM_DIZZY);
		}
		else
		{
			SetFlag(CRE_FLAG_INVUL, false);

			if(TimeElapse(&m_dizzyDelay))
				ChangeBehavior(SHROOMPA_STATE_WANDER);
		}
		break;
	}
}

int ShroomPa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			char buff[MAXCHARBUFF]={0};
			hCFG cfg = (hCFG)wParam;

			m_normSpd = GetMoveSpd();

			///////////////////////////////////////////////////////////////
			//get the 'special' data
			TimeInit(&m_actionDelay, CfgGetItemFloat(cfg, "special", "behaviorDelay"));

			TimeInit(&m_dizzyDelay, CfgGetItemFloat(cfg, "special", "dizzyDelay"));

			TimeInit(&m_dizzyAfterHitDelay, CfgGetItemFloat(cfg, "special", "dizzyAfterHitDelay"));

			///////////////////////////////////////////////////////////////
			//get the 'behavior' data

			for(int i = 0; i < SHROOMPA_STATE_MAX; i++)
			{
				sprintf(buff, "%d", i);

				m_actionChance[i] = CfgGetItemInt(cfg, "behavior", buff);
			}
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			//get the 'spit' information

			//Load the model for spit
			if(CfgGetItemStr(cfg, "spit", "model", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_spitMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "spit", "trail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_spitFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load color
			if(CfgGetItemStr(cfg, "spit", "clr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_r=r; m_g=g; m_b=b;
			}

			//load attack stuff
			m_spitSpd = CfgGetItemFloat(cfg, "spit", "spd");

			m_spitMax = CfgGetItemInt(cfg, "spit", "max");

			m_spitAngleStart = CfgGetItemFloat(cfg, "spit", "angleStart");
			m_spitAngleStart = D3DXToRadian(m_spitAngleStart);

			m_spitAngleInc = CfgGetItemFloat(cfg, "spit", "angleMax");
			m_spitAngleInc /= m_spitMax;
			m_spitAngleInc = D3DXToRadian(m_spitAngleInc);
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			//get the 'charge' information

			m_chargeSpd = CfgGetItemFloat(cfg, "charge", "spd");
			
			TimeInit(&m_chargeDelay, CfgGetItemFloat(cfg, "charge", "delay"));
			///////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////
			//FX thing
			fxGas_init tFX;

			if(CfgGetItemStr(cfg, "FX", "txt", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				tFX.gasTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			if(CfgGetItemStr(cfg, "FX", "clr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				tFX.r = r; tFX.g = g; tFX.b = b;
			}

			if(CfgGetItemStr(cfg, "FX", "dir", buff))
				sscanf(buff, "%f,%f,%f", &tFX.dir[eX],&tFX.dir[eY],&tFX.dir[eZ]);

			tFX.spd = CfgGetItemFloat(cfg, "FX", "spd");

			tFX.spawnDelay = CfgGetItemFloat(cfg, "FX", "spawnDelay");

			memcpy(tFX.min, (float*)BoundGetMin(), sizeof(float)*eMaxPt);
			memcpy(tFX.max, (float*)BoundGetMax(), sizeof(float)*eMaxPt);

			tFX.max[eY] /= 4;

			tFX.maxParticle = CfgGetItemInt(cfg, "FX", "max");
			
			tFX.scaleMax = CfgGetItemFloat(cfg, "FX", "scale");

			tFX.scaleDelayMin = CfgGetItemFloat(cfg, "FX", "scaleDelayMin");
			tFX.scaleDelayMax = CfgGetItemFloat(cfg, "FX", "scaleDelayMax");

			tFX.alphaDelayMin = CfgGetItemFloat(cfg, "FX", "alphaDelayMin");
			tFX.alphaDelayMax = CfgGetItemFloat(cfg, "FX", "alphaDelayMax");

			tFX.bRepeat = true;

			m_particle = PARFXCreate(ePARFX_GAS, &tFX, -1, GetOBJ(), -1, 0);
			///////////////////////////////////////////////////////////////
		}
		break;

	case ENTITYMSG_UPDATE:
		//we are always invulnerable!!!
		SetFlag(CRE_FLAG_INVUL, true);

		//don't do anything if we have no waypoint
		if(!GetWayPoint())
		{
			if(m_particle)
				PARFXActivate(m_particle, false);

			break;
		}

		//if we are not walking
		if(m_actionState != SHROOMPA_STATE_WANDER)
		{
			//don't move
			SetLoc(GetLoc());
			D3DXVECTOR3 vel =GetVel();
			vel.x = vel.z = 0;
			SetVel(vel);
		}
		else if(m_bGoToDest)
		{
			D3DXVECTOR3 destVec = m_dest - GetLoc();

			float len = D3DXVec3Length(&destVec);

			float spd = GetMove().MoveUpdate(g_timeElapse);
			
			if(len > spd)
			{
				destVec /= len;

				SetDir(destVec);

				destVec *= spd;	
			}
			//we are close enough
			else
				m_bGoToDest = false;

			destVec.y = GetVel().y;
			SetVel(destVec);
		}
		
		//check for some other stuff
		BehaviorUpdate(false);

		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity)
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
					&& m_actionState != SHROOMPA_STATE_DIZZY)
				{
					//hit this fella
					Creature *pCre = (Creature *)pEntity;

					if(pCre->Hit())
						pCre->CREPlaySound(5); //play hit hard sound
				}
				else if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
				{
					if(pEntity->GetSubType() == PROJ_SPIT)
						ChangeBehavior(SHROOMPA_STATE_DIZZY);
				}
			}
		}
		break;

	case CREMSG_HIT:
		m_bDizzyAfterHit = true;
		TimeReset(&m_dizzyAfterHitDelay);
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 29;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}

int ShroomPa::GetDeathState()
{
	return SHROOMPA_ANIM_DEATH;
}

int ShroomPa::GetOuchState()
{
	return SHROOMPA_ANIM_OUCH;
}

void ShroomPa::MoveAnim(eMoveType type)
{
	int OBJState = OBJGetCurState(GetOBJ());

	if(OBJState == SHROOMPA_ANIM_IDLE
		|| OBJState == SHROOMPA_ANIM_WALK)
	{
		switch(type)
		{
		case MOVE_FALL:
			OBJSetState(GetOBJ(), SHROOMPA_ANIM_WALK);
			break;

		case MOVE_NONE:
			OBJSetState(GetOBJ(), SHROOMPA_ANIM_IDLE);
			break;
		
		case MOVE_WALK:
			OBJSetState(GetOBJ(), SHROOMPA_ANIM_WALK);
			break;
		}
	}
	else if(OBJState == SHROOMPA_ANIM_OUCH && OBJIsStateEnd(GetOBJ()))
		OBJSetState(GetOBJ(), SHROOMPA_ANIM_IDLE);
}