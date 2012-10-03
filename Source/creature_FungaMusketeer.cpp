#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	FUNGAMUSKETEER_ANIM_IDLE,
	FUNGAMUSKETEER_ANIM_WALK,
	FUNGAMUSKETEER_ANIM_OUCH,
	FUNGAMUSKETEER_ANIM_DEATH,
	FUNGAMUSKETEER_ANIM_FIRE,
	FUNGAMUSKETEER_ANIM_RELOAD,
	FUNGAMUSKETEER_ANIM_BLOCK
} eShroomGuardAnim;

FungaMusketeer::FungaMusketeer() : EnemyCommon(ENEMY_FUNGAMUSKETEER),
m_attackRadiusSQ(0), m_bulletSpd(0), m_bulletMdl(0), m_bulletFXTxt(0),
m_fixedDir(0,0,0)
{
}

FungaMusketeer::~FungaMusketeer()
{
	if(m_bulletMdl)
		MDLDestroy(&m_bulletMdl);

	if(m_bulletFXTxt)
		TextureDestroy(&m_bulletFXTxt);
}

int FungaMusketeer::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			char buff[MAXCHARBUFF]={0};
			hCFG cfg = (hCFG)wParam;

			m_attackRadiusSQ = CfgGetItemFloat(cfg, "special", "attackRange");
			m_attackRadiusSQ *= m_attackRadiusSQ;

			//Load the model for spit
			if(CfgGetItemStr(cfg, "special", "attackModel", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_bulletMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "special", "attackTrail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_bulletFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load color
			if(CfgGetItemStr(cfg, "special", "attackClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_r=r; m_g=g; m_b=b;
			}

			//load attack stuff
			m_bulletSpd = CfgGetItemFloat(cfg, "special", "attackSpd");

			TimeInit(&m_blockDelay, CfgGetItemFloat(cfg, "special", "blockDelay"));
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int state = OBJGetCurState(GetOBJ());

			//if we are not walking...
			if(state != FUNGAMUSKETEER_ANIM_IDLE
				&& state != FUNGAMUSKETEER_ANIM_WALK)
			{
				//don't move
				D3DXVECTOR3 vel =GetVel();
				vel.x = vel.z = 0;
				SetVel(vel);

				//set to fixed direction
				SetDir(m_fixedDir);
			}

			switch(state)
			{
			case FUNGAMUSKETEER_ANIM_IDLE:
			case FUNGAMUSKETEER_ANIM_WALK:
				{
					m_fixedDir = GetDir();

					//look for nearest Ta-Ta
					D3DXVECTOR3 vec, vecMin;
					float vecLenSq, vecLenMin = m_attackRadiusSQ;
					Creature *pCre, *pCreMin=0;
					
					for(int i = 0; i < MAXTATAWORLD; i++)
					{
						pCre = (Creature *)IDPageQuery(g_world->TataGet(i));

						if(pCre
							&& !pCre->CheckFlag(TATA_FLAG_ENEMY_IGNORE))
						{
							vec = pCre->GetLoc() - GetLoc();
							vecLenSq = D3DXVec3LengthSq(&vec);

							//uh oh, better run!
							//check to be sure that the player is 
							//within height range
							if(vecLenSq <= vecLenMin
								&& pCre->GetLoc().y >= (GetLoc().y + BoundGetMin().y + pCre->BoundGetMin().y)
								&& pCre->GetLoc().y <= (GetLoc().y + BoundGetMax().y + pCre->BoundGetMax().y))
							{
								vecMin    = vec;
								vecLenMin = vecLenSq;
								pCreMin   = pCre;
							}
						}
					}

					//did we find anyone near?
					if(pCreMin)
					{
						float len = sqrtf(vecLenMin);
						vecMin /= len;
						vecMin.y = 0;

						//face the Ta-Ta
						SetDir(vecMin);
						m_fixedDir = vecMin;

						//FIRE!!!
						OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_FIRE);
					}
				}
				break;

			case FUNGAMUSKETEER_ANIM_FIRE:
				if(OBJIsStateEnd(GetOBJ()))
				{
					//create projectile
					if(m_bulletMdl)
					{
						D3DXVECTOR3 bLoc;
						OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "weapon_start"), (float*)bLoc);
						new Spit(GetID(), bLoc, m_bulletSpd, m_bulletMdl, 
							m_bulletFXTxt, m_r, m_g, m_b);

						fxExplode_init tFX;

						tFX.explodeTxt = m_bulletFXTxt;
						tFX.r = m_r; tFX.g = m_g; tFX.b = m_b;
						tFX.scale = 1;
						tFX.radius = 20;
						tFX.maxParticle = 10;
						tFX.delay = 500;

						PARFXCreate(ePARFX_EXPLODE, &tFX, -1, 
						  GetOBJ(), OBJJointGetIndex(GetOBJ(), "weapon_start"), 0);
					}

					//play gunfire sound
					CREPlaySound(23);

					//reload
					OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_RELOAD);

					//play reload sound
					CREPlaySound(24);
				}
				break;

			case FUNGAMUSKETEER_ANIM_RELOAD:
				//done reloading? if so, then get back to walking
				if(OBJIsStateEnd(GetOBJ()))
					OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_IDLE);
				break;

			case FUNGAMUSKETEER_ANIM_BLOCK:
				if(OBJIsStateEnd(GetOBJ()) && TimeElapse(&m_blockDelay))
				{
					OBJPauseOnStateEnd(GetOBJ(), false);
					OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_IDLE);
				}
				break;
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			
			int OBJState = OBJGetCurState(m_obj);

			//check to see if it is a projectile (spit/melee) and that we are blocking
			if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
				//&& (pEntity->GetSubType() == PROJ_SPIT || pEntity->GetSubType() == PROJ_ATTACKMELEE))
			{
				Projectile *pProj = (Projectile *)pEntity;

				if(OBJState == FUNGAMUSKETEER_ANIM_BLOCK)
				{
					float nd = D3DXVec3Dot(&GetDir(), &pProj->GetDir());

					//make sure we are facing the projectile
					if(nd <= 0)
					{
						//reflect the projectile
						if(pEntity->GetSubType() == PROJ_SPIT)
						{
							//change the projectile's owner 
							pProj->SetOwner(GetID());

							D3DXVECTOR3 refl;

							refl = pProj->GetDir() - (2*nd*GetDir());

							pProj->SetDir(refl);
						}
						else
						{
							EntityCommon *pEntityOwner = (EntityCommon *)IDPageQuery(pProj->GetOwner());
							if(pEntityOwner)
							{
								hOBJ obj = pEntityOwner->GetOBJ();
								if(obj)
									OBJSetState(obj, 0);
							}

							//set to no owner
							Id invID = {0,-1};
							pProj->SetOwner(invID);
						}
					}
				}
				else
				{
					//BLOCK!!!
					TimeReset(&m_blockDelay);
					OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_BLOCK);
					OBJPauseOnStateEnd(GetOBJ(), true);

					//play shield 'on' sound
					CREPlaySound(0);

					if(pEntity->GetSubType() != PROJ_SPIT)
					{
						EntityCommon *pEntityOwner = (EntityCommon *)IDPageQuery(pProj->GetOwner());
						if(pEntityOwner)
						{
							hOBJ obj = pEntityOwner->GetOBJ();
							if(obj)
								OBJSetState(obj, 0);
						}
					}

					//set to no owner
					Id invID = {0,-1};
					pProj->SetOwner(invID);
					pProj->SetFlag(ENTITY_FLAG_POLLDEATH, true);
				}
			}
		}
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 20;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}

int FungaMusketeer::GetDeathState()
{
	return FUNGAMUSKETEER_ANIM_DEATH;
}

int FungaMusketeer::GetOuchState()
{
	return FUNGAMUSKETEER_ANIM_OUCH;
}

void FungaMusketeer::MoveAnim(eMoveType type)
{
	int OBJState = OBJGetCurState(GetOBJ());

	if(OBJState == FUNGAMUSKETEER_ANIM_IDLE
		|| OBJState == FUNGAMUSKETEER_ANIM_WALK)
	{
		switch(type)
		{
		case MOVE_FALL:
			OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_WALK);
			break;

		case MOVE_NONE:
			OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_IDLE);
			break;
		
		case MOVE_WALK:
			OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_WALK);
			break;
		}
	}
	else if(OBJState == FUNGAMUSKETEER_ANIM_OUCH && OBJIsStateEnd(GetOBJ()))
		OBJSetState(GetOBJ(), FUNGAMUSKETEER_ANIM_IDLE);
}