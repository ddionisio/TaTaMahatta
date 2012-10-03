#include "tata_main.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

//Ta-Ta Tug interface

TaTaTug::~TaTaTug()
{
}

int TaTaTug::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_pushState = CfgGetItemInt(cfg, "special", "pushState");

			if(m_pushState == -1)
				m_pushState = 0;

			m_pushSpd = CfgGetItemFloat(cfg, "special", "pushSpd");

			if(m_pushSpd == -1)
				m_pushSpd = 0;
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState == m_pushState && OBJIsStateEnd(m_obj))
			{
				OBJSetState(m_obj, CREANIM_IDLE1);
			}
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_RELEASED)
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState != m_pushState)
			{
				OBJSetState(m_obj, m_pushState);

				//play push sound
				CREPlaySound(2);
				
				//create push projectile
				new PushMelee(GetID(), m_pushSpd);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}