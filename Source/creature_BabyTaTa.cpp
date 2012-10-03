#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

BabyTaTa::~BabyTaTa()
{
}

int BabyTaTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_hugState = CfgGetItemInt(cfg, "special", "hugState");

			if(m_hugState == -1)
				m_hugState = 0;
		}
		break;

	case ENTITYMSG_UPDATE:
		break;

	case CREMSG_ACTION:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			//attach ourself to this fellow Ta-Ta,
			//also make sure it is on the playlist
			if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
				&& g_world->TataOnPlayList(pEntity->GetName()))
			{
				TaTaCommon *pTata = (TaTaCommon *)pEntity;
				//make sure nothing is attached
				if(!pTata->IsAttach())
					pTata->AttachTaTa(this, m_hugState);
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}