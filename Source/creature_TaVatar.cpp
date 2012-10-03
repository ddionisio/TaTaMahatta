#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

/*
int			m_atkPrepState;			//attack prep. animation state
	int			m_atkState;				//attack animation state

	hMDL		m_fBallMdl;			//the model of the fireball
	hTXT		m_fBallFXTxt;		//the fireball particle FX texture
	BYTE		m_r,m_g,m_b;		//particle color
	
	float		m_attackSpd;		//the speed of the projectile

	hPARFX		m_fxCharge;				//the charge FX while preparing to attack
*/

TaVatar::TaVatar() : TaTaCommon(TATA_TAVATAR), m_atkPrepState(0),
m_atkState(0), m_fBallMdl(0), m_fBallFXTxt(0), m_fxCharge(0),
m_bChargeHasEnd(false), m_bFire(false)
{
}

TaVatar::~TaVatar()
{
	if(m_fBallMdl)
		MDLDestroy(&m_fBallMdl);

	if(m_fBallFXTxt)
		TextureDestroy(&m_fBallFXTxt);

	if(m_fxCharge)
		PARFXDestroy(&m_fxCharge);
}

int TaVatar::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;
			char buff[MAXCHARBUFF]={0};

			////////////////////////////////////////////////
			//load state stuff
			m_atkPrepState = CfgGetItemInt(cfg, "special", "atkPrepState"); if(m_atkPrepState < 0) m_atkPrepState = 0;
			m_atkState = CfgGetItemInt(cfg, "special", "atkState"); if(m_atkState < 0) m_atkState = 0;

			////////////////////////////////////////////////
			//load fireball

			//Load the model for spit
			if(CfgGetItemStr(cfg, "special", "attackModel", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_fBallMdl = MDLCreate(0, mdlPath.c_str());
			}

			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "special", "attackTrail", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_fBallFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load color
			if(CfgGetItemStr(cfg, "special", "attackClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_r=r; m_g=g; m_b=b;
			}

			//load attack stuff
			m_fBallRadius = CfgGetItemFloat(cfg, "special", "attackRadius");
			m_attackSpd = CfgGetItemFloat(cfg, "special", "attackSpd");

			////////////////////////////////
			// Charge FX
			fxGather_init m_fxChargeDat;
			
			//Load the texture for spit trails
			if(CfgGetItemStr(cfg, "chargeFX", "texture", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_fxChargeDat.explodeTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//load color
			if(CfgGetItemStr(cfg, "chargeFX", "attackClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_fxChargeDat.r=r; m_fxChargeDat.g=g; m_fxChargeDat.b=b;
			}

			m_fxChargeDat.scale = CfgGetItemFloat(cfg, "chargeFX", "scale");
			m_fxChargeDat.radius = CfgGetItemFloat(cfg, "chargeFX", "radius");
			m_fxChargeDat.maxParticle = CfgGetItemInt(cfg, "chargeFX", "max");

			m_fxChargeDat.delayGather = CfgGetItemFloat(cfg, "chargeFX", "delayGather");
			m_fxChargeDat.delayFade = CfgGetItemFloat(cfg, "chargeFX", "delayFade");
			m_fxChargeDat.delaySpawn = CfgGetItemFloat(cfg, "chargeFX", "delaySpawn");

			//create the FX
			int chargeJointInd = OBJJointGetIndex(GetOBJ(), "fireball");
			m_fxCharge = PARFXCreate(ePARFX_GATHER, &m_fxChargeDat, -1, 
						  GetOBJ(), chargeJointInd, 0);

			//deactivate for now
			PARFXActivate(m_fxCharge, false);
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int objState = OBJGetCurState(GetOBJ());

			if(m_bFire 
				&& m_bChargeHasEnd
				&& objState == m_atkPrepState
				&& OBJIsStateEnd(GetOBJ()))
			{
				m_bFire = false;

				PARFXActivate(m_fxCharge, false);

				OBJSetState(GetOBJ(), m_atkState);
			}

			if(OBJIsStateEnd(GetOBJ()))
			{
				if(objState == m_atkPrepState)
					m_bChargeHasEnd = true;
				else if(objState == m_atkState)
				{
					D3DXVECTOR3 spitLoc;
					OBJJointGetWorldLoc(GetOBJ(), OBJJointGetIndex(GetOBJ(), "fireball"), (float*)spitLoc);

					new Spit2(GetID(), spitLoc, m_attackSpd, m_fBallMdl, 
						m_fBallFXTxt, m_fBallRadius, m_r, m_g, m_b,
						false, 0, 42);

					OBJSetState(GetOBJ(), CREANIM_IDLE1);
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case CREMSG_ACTION:
		{
			int objState = OBJGetCurState(GetOBJ());

			//CHARGE!
			if(wParam == INP_STATE_DOWN
				&& objState != m_atkPrepState
				&& objState != m_atkState)
			{
				m_bChargeHasEnd = false;
				OBJSetState(GetOBJ(), m_atkPrepState);

				PARFXActivate(m_fxCharge, true);
			}
			//ATTACK!
			else if(wParam == INP_STATE_RELEASED)
			{
				m_bFire = true;
			}
		}
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 44;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}