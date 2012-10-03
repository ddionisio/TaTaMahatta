#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

LoopTaLoop::LoopTaLoop() : TaTaCommon(TATA_LOOPTALOOP), m_attackState(0), 
m_attackEndState(0), m_grappleState(0), m_whipLen(0), m_whipLenMax(0), 
m_attackBound(0,0,0), m_stun(0)
{
	m_proj.counter = -1;
}

LoopTaLoop::~LoopTaLoop()
{
}

int LoopTaLoop::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_attackState = CfgGetItemInt(cfg, "special", "attackState");

			if(m_attackState == -1)
				m_attackState = 0;

			m_attackEndState = CfgGetItemInt(cfg, "special", "attackEndState");

			if(m_attackEndState == -1)
				m_attackEndState = 0;

			m_grappleState = CfgGetItemInt(cfg, "special", "grappleState");

			if(m_grappleState == -1)
				m_grappleState = 0;

			//get whip speed
			m_whipSpd.MoveSetSpeed(CfgGetItemFloat(cfg, "special", "whipSpd"));

			//get whip lenght
			m_whipLenMax = CfgGetItemFloat(cfg, "special", "whipLength");

			//get the attack bound m_attackBound
			m_attackBound.x = CfgGetItemFloat(cfg, "special", "attackBoundX");
			m_attackBound.y = CfgGetItemFloat(cfg, "special", "attackBoundY");
			m_attackBound.z = CfgGetItemFloat(cfg, "special", "attackBoundZ");

			//get stun delay
			m_stun = CfgGetItemFloat(cfg, "special", "attackStunDelay");

		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState == m_attackState && OBJIsStateEnd(m_obj))
			{
				OBJSetState(m_obj, m_attackEndState);

				//play whip crack sound
				CREPlaySound(8);

				//create the whip projectile
				Whip *pWhip = new Whip(GetID(), 
					OBJJointGetIndex(GetOBJ(), "whip_end"), 
					m_whipLenMax, m_whipSpd.MoveGetSpeed(), 
					m_stun, m_grappleState);

				if(pWhip)
					m_proj = pWhip->GetID();
			}
			else if(OBJState == m_attackEndState
				|| OBJState == m_grappleState)
			{
				Whip *pWhip = (Whip *)IDPageQuery(m_proj);

				if(!pWhip)
				{
					//no steak hit and that whip has finished
					m_proj.counter = -1;

					//get back to idle
					OBJSetState(m_obj, CREANIM_IDLE1);
				}
			}
			else if(OBJState != m_attackState
				&& OBJState != m_attackEndState
				&& OBJState != m_grappleState)
				SetFlag(TATA_FLAG_DISABLE_ROTATE, false);
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_RELEASED)
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState != m_attackState
				&& OBJState != m_attackEndState
				&& OBJState != m_grappleState)
			{
				OBJSetState(m_obj, m_attackState);

				SetFlag(TATA_FLAG_DISABLE_ROTATE, true);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_STAKE:
			*((int*)lParam) = 9;
			break;

		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 10;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}
