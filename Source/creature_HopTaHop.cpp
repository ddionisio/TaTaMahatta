#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

HopTaHop::~HopTaHop()
{
}

void HopTaHop::InputStrafe()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK)
		|| (tataOBJState == CREANIM_FALL) 
		|| (tataOBJState == m_jumpState))
	{
		if(InputIsDown(INP_LEFT))
			MoveStrafe(-0.5f);
		else if(InputIsDown(INP_RIGHT))
			MoveStrafe(0.5f);
	}
}

int HopTaHop::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_jumpState = CfgGetItemInt(cfg, "special", "jumpState");

			if(m_jumpState == -1)
				m_jumpState = 0;

			//get jump speed
			m_jumpSpd.MoveSetSpeed(CfgGetItemFloat(cfg, "special", "jumpSpd"));
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState == m_jumpState)
			{
				SetLoc(GetLoc());

				D3DXVECTOR3 vel = GetVel();

				vel.y = m_jumpSpd.MoveUpdate(g_timeElapse);

				SetVel(vel);

				if(OBJIsStateEnd(m_obj))
				{
					m_bJump = true;
					OBJSetState(m_obj, CREANIM_FALL);
				}
			}
			//once landed, we don't want air control anymore
			else if(OBJState != CREANIM_FALL)
				m_bJump = false;

			//allow air control while jumping
			if(m_bJump
				&& GAME_IDISEQUAL(g_world->TataGetCurrent(), GetID()))
			{
				if(InputIsDown(INP_UP))
					MoveDir(1);
				else if(InputIsDown(INP_DOWN))
					MoveDir(-1);
			}
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_RELEASED)
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState != m_jumpState)
			{
				OBJSetState(m_obj, m_jumpState);
				//m_bJump = true;

				//play boing sound
				CREPlaySound(21);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}