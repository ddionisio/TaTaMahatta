#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

ChiTa::ChiTa() : TaTaCommon(TATA_CHITA), m_pullState(0), m_projBound(0,0,0), 
m_pullSpd(0), m_pullDist(0), m_pullSnd(0)
{
	m_projID.counter = -1;
}

ChiTa::~ChiTa()
{
	if(m_pullSnd)
		BASS_ChannelStop(m_pullSnd);
}

int ChiTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_pullState = CfgGetItemInt(cfg, "special", "pullState");

			if(m_pullState == -1)
				m_pullState = 0;

			m_projBound.x = CfgGetItemFloat(cfg, "special", "pullBoundX");
			m_projBound.y = CfgGetItemFloat(cfg, "special", "pullBoundY");
			m_projBound.z = CfgGetItemFloat(cfg, "special", "pullBoundZ");

			m_pullSpd = CfgGetItemFloat(cfg, "special", "pullSpd");

			m_pullDist = CfgGetItemFloat(cfg, "special", "pullDist");
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			Projectile *pProj = (Projectile *)IDPageQuery(m_projID);

			int objState = OBJGetCurState(m_obj);
			
			if(objState != m_pullState)
			{
				//destroy projectile
				if(pProj)
				{
					pProj->SetFlag(ENTITY_FLAG_POLLDEATH, true);

					m_projID.counter = -1;
				}

				if(m_pullSnd)
				{ BASS_ChannelStop(m_pullSnd); m_pullSnd = 0; }
			}
			else if(!pProj && objState == m_pullState)
			{
				OBJSetState(GetOBJ(), CREANIM_IDLE1);
				m_projID.counter = -1;

				if(m_pullSnd)
				{ BASS_ChannelStop(m_pullSnd); m_pullSnd = 0; }
			}
		}
		break;

	case CREMSG_ACTION:
		{
			int objState = OBJGetCurState(GetOBJ());

			Projectile *pProj = (Projectile *)IDPageQuery(m_projID);

			if(wParam == INP_STATE_DOWN
				&& objState != m_pullState)
			{
				OBJSetState(GetOBJ(), m_pullState);

				//create projectile
				if(!pProj)
				{
					pProj = new PullPower(GetID(), m_projBound, m_pullDist, m_pullSpd);

					m_projID = pProj->GetID();

					//play sonar thingy sound
					m_pullSnd = CREPlaySound(13);
				}
			}
			else if(wParam == INP_STATE_RELEASED
				&& objState == m_pullState)
			{
				OBJSetState(GetOBJ(), CREANIM_IDLE1);

				if(pProj)
				{
					pProj->SetFlag(ENTITY_FLAG_POLLDEATH, true);
					m_projID.counter = -1;
				}

				if(m_pullSnd)
				{ BASS_ChannelStop(m_pullSnd); m_pullSnd = 0; }
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}