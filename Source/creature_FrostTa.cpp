#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

/*
class FrostTa : public TaTaCommon {
public:
	FrostTa();
	~FrostTa();

	int Callback(unsigned int msg, unsigned int wParam, int lParam);

private:
	int				m_attackState;			//gun attack animation state
	D3DXVECTOR3		m_attackBound;			//the max attack bound
	hTXT			m_frostFXTxt;			//frost gas texture
	BYTE			m_r,m_g,m_b;			//color of frost gas
};*/
FrostTa::FrostTa() : TaTaCommon(TATA_FROSTTA), m_attackState(0), 
m_attackBound(0,0,0), m_frostFXTxt(0), m_frozenMdl(0), m_frozenTxt(0),
m_frostChan(0)
{
	m_proj.counter = -1;
}

FrostTa::~FrostTa()
{
	if(m_frostFXTxt)
		TextureDestroy(&m_frostFXTxt);

	if(m_frozenMdl)
		MDLDestroy(&m_frozenMdl);

	if(m_frozenTxt)
		TextureDestroy(&m_frozenTxt);
}

int FrostTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_attackState = CfgGetItemInt(cfg, "special", "attackState");

			if(m_attackState == -1)
				m_attackState = 0;

			//get the attack bound
			m_attackBound.x = CfgGetItemFloat(cfg, "special", "attackBoundX");
			m_attackBound.y = CfgGetItemFloat(cfg, "special", "attackBoundY");
			m_attackBound.z = CfgGetItemFloat(cfg, "special", "attackBoundZ");

			char buff[MAXCHARBUFF]={0};

			//get frozen texture
			if(CfgGetItemStr(cfg, "special", "frostFXTxt", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_frostFXTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}

			//get frozen color
			if(CfgGetItemStr(cfg, "special", "frostFXClr", buff))
			{
				int r,g,b;
				sscanf(buff, "%d,%d,%d", &r,&g,&b);
				m_r = r; m_g = g; m_b = b;
			}

			//get frozen model chunk
			if(CfgGetItemStr(cfg, "special", "frostMDL", buff))
			{
				string mdlPath = GAMEFOLDER;
				mdlPath += "\\";
				mdlPath += buff;

				m_frozenMdl = MDLCreate(0, mdlPath.c_str());
			}

			//get frozen texture
			if(CfgGetItemStr(cfg, "special", "frostTxt", buff))
			{
				string txtPath = GAMEFOLDER;
				txtPath += "\\";
				txtPath += buff;

				m_frozenTxt = TextureCreate(0, txtPath.c_str(), false, 0);
			}
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int objState = OBJGetCurState(m_obj);
			
			if(objState != m_attackState)
			{
				//destroy projectile
				Projectile *pProj = (Projectile *)IDPageQuery(m_proj);

				if(pProj)
				{
					pProj->SetFlag(ENTITY_FLAG_POLLDEATH, true);

					m_proj.counter = -1;

					//stop freeze sound
					if(m_frostChan)
					{
						BASS_ChannelStop(m_frostChan);
						m_frostChan = 0;
					}
				}
			}
		}
		break;

	case CREMSG_ACTION:
		{
			int objState = OBJGetCurState(m_obj);

			if(wParam == INP_STATE_DOWN
				&& objState != m_attackState)
			{
				//set state
				OBJSetState(m_obj, m_attackState);

				//create projectile
				Projectile *pProj = (Projectile *)IDPageQuery(m_proj);

				if(!pProj)
				{
					pProj = new Frost(GetID(), m_attackBound, 
						OBJJointGetIndex(m_obj, "freeze_start"), 
						OBJJointGetIndex(m_obj, "freeze_end"), 
						m_frostFXTxt, m_frozenMdl, m_frozenTxt, 
						m_r, m_g, m_b);

					//play frost sound
					m_frostChan = CREPlaySound(11);

					m_proj = pProj->GetID();
				}
			}
			//button was released
			else if(wParam == INP_STATE_RELEASED
				&& objState == m_attackState)
			{
				//set state
				OBJSetState(m_obj, CREANIM_IDLE1);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}