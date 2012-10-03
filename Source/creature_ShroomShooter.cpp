#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	SHROOMSHOOTER_ANIM_IDLE1,
	SHROOMSHOOTER_ANIM_IDLE2,
	SHROOMSHOOTER_ANIM_IDLE_MAX = SHROOMSHOOTER_ANIM_IDLE2,
	SHROOMSHOOTER_ANIM_SHOOT,
	SHROOMSHOOTER_ANIM_OUCH,
	SHROOMSHOOTER_ANIM_DEATH
} eShroomShooterAnim;

ShroomShooter::ShroomShooter() : EnemyCommon(ENEMY_SHROOMSHOOTER),
	m_spitMdl(0), m_spitFXTxt(0), m_attackSpd(0)
{
}

ShroomShooter::~ShroomShooter()
{
	if(m_spitMdl)
		MDLDestroy(&m_spitMdl);

	if(m_spitFXTxt)
		TextureDestroy(&m_spitFXTxt);
}

int ShroomShooter::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			char buff[MAXCHARBUFF]={0};
			hCFG cfg = (hCFG)wParam;

			//Load the model for spit
			if(CfgGetItemStr(cfg, "special", "attackModel", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_spitMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "special", "attackTrail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_spitFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load color
			if(CfgGetItemStr(cfg, "special", "attackClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_r=r; m_g=g; m_b=b;
			}

			//load attack stuff
			m_attackSpd = CfgGetItemFloat(cfg, "special", "attackSpd");

			double atkDelay = CfgGetItemFloat(cfg, "special", "attackDelay");

			TimeInit(&m_attackDelay, atkDelay);
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int state = OBJGetCurState(GetOBJ());

			if(state != SHROOMSHOOTER_ANIM_SHOOT
				&& state != SHROOMSHOOTER_ANIM_OUCH)
			{
				if(TimeElapse(&m_attackDelay))
				{
					//set to attack state
					OBJSetState(GetOBJ(), SHROOMSHOOTER_ANIM_SHOOT);

					//spit sound
					//CREPlaySound(19);
				}
			}

			if(OBJIsStateEnd(GetOBJ()))
			{
				if(state == SHROOMSHOOTER_ANIM_SHOOT)
				{
					//create spit projectile
					if(m_spitMdl)
					{
						D3DXVECTOR3 spitLoc;
						OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "mouth"), (float*)spitLoc);
						new Spit(GetID(), spitLoc, m_attackSpd, m_spitMdl, 
							m_spitFXTxt, m_r, m_g, m_b);
					}
				}

				//random idle
				int newIdleState = Random(SHROOMSHOOTER_ANIM_IDLE1, 
					SHROOMSHOOTER_ANIM_IDLE_MAX);

				OBJSetState(GetOBJ(), newIdleState);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
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

int ShroomShooter::GetDeathState()
{
	return SHROOMSHOOTER_ANIM_DEATH;
}

int ShroomShooter::GetOuchState() 
{ 
	return SHROOMSHOOTER_ANIM_OUCH; 
}