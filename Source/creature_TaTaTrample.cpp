#include "tata_main.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

TaTaTrample::~TaTaTrample()
{
}

int TaTaTrample::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_attackPrepState = CfgGetItemInt(cfg, "special", "attackPrepareState");
			m_attackState = CfgGetItemInt(cfg, "special", "attackState");

			m_attack2PrepState = CfgGetItemInt(cfg, "special", "attackPrepare2State");
			m_attack2State = CfgGetItemInt(cfg, "special", "attack2State");

			if(m_attackState == -1)
				m_attackState = 0;

			//get the attack bound m_attackBound
			m_attackBound.x = CfgGetItemFloat(cfg, "special", "attackBoundX");
			m_attackBound.y = CfgGetItemFloat(cfg, "special", "attackBoundY");
			m_attackBound.z = CfgGetItemFloat(cfg, "special", "attackBoundZ");
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJIsStateEnd(m_obj)
				&& (OBJState == m_attackPrepState || OBJState == m_attack2PrepState))
			{
				if(OBJState == m_attackPrepState)
					OBJSetState(m_obj, m_attackState);
				else
					OBJSetState(m_obj, m_attack2State);

				//play swing melee sound
				CREPlaySound(3);

				//create attack projectile
				new AttackMelee(GetID(), m_attackBound, OBJJointGetIndex(m_obj, "tip_b"));
			}

			if((OBJState == m_attackState || OBJState == m_attack2State) && OBJIsStateEnd(m_obj))
			{
				OBJSetState(m_obj, CREANIM_IDLE1);
			}
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_RELEASED)
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState != m_attackPrepState && OBJState != m_attackState
				&& OBJState != m_attack2PrepState && OBJState != m_attack2State)
			{
				if(InputIsDown(INP_UP))
					OBJSetState(m_obj, m_attack2PrepState);
				else
					OBJSetState(m_obj, m_attackPrepState);
			}
		}
		break;

	case ENTITYMSG_REQUESTSOUND:
		switch(wParam)
		{
		case SND_REQ_PROJ_HIT_WALL:
			*((int*)lParam) = 4;
			break;

		case SND_REQ_PROJ_HIT_CRE:
			*((int*)lParam) = 5;
			break;
		}
		break;
	}

	return RETCODE_SUCCESS;
}