#include "tata_main.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

//Tinker Ta interface

TinkerTa::~TinkerTa()
{
}

int TinkerTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_useState = CfgGetItemInt(cfg, "special", "useState");

			if(m_useState == -1)
				m_useState = 0;

			//get the use bound
			m_useBound.x = CfgGetItemFloat(cfg, "special", "useBoundX");
			m_useBound.y = CfgGetItemFloat(cfg, "special", "useBoundY");
			m_useBound.z = CfgGetItemFloat(cfg, "special", "useBoundZ");
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState == m_useState && OBJIsStateEnd(m_obj))
			{
				OBJSetState(m_obj, CREANIM_IDLE1);
			}
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_RELEASED)
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState != m_useState)
			{
				OBJSetState(m_obj, m_useState);

				//play swing wrench sound
				CREPlaySound(6);
				
				//create use projectile
				new UseMelee(GetID(), m_useBound, 
					OBJJointGetIndex(m_obj, "wrench_b"),
					OBJJointGetIndex(m_obj, "wrench_e"));
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}