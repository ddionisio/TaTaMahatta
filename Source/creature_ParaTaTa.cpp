#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

//CRE_FLAG_GASIMMUNE
ParaTaTa::ParaTaTa() : TaTaCommon(TATA_PARATATA), m_glideState(0), m_bGlide(false) 
{
	SetFlag(CRE_FLAG_GASIMMUNE, true);
}

ParaTaTa::~ParaTaTa()
{
}

int ParaTaTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_glideState = CfgGetItemInt(cfg, "special", "glideState");

			if(m_glideState == -1)
				m_glideState = 0;

			//get jump speed
			m_glideSpd.MoveSetSpeed(CfgGetItemFloat(cfg, "special", "glideSpd"));
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState == m_glideState)
			{
				SetLoc(GetLoc());
				
				D3DXVECTOR3 vel = GetVel();

				if(vel.y < 0)
					vel.y = -m_glideSpd.MoveUpdate(g_timeElapse);

				SetVel(vel);
			}

			m_bGlide = InputIsDown(INP_A) ? true : false;
		}
		break;

	case CREMSG_ACTION:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;
	}

	return RETCODE_SUCCESS;
}

void ParaTaTa::InputWalk()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK)
		|| (tataOBJState == m_glideState))
	{
		//check to see if the 'up'/'down' button is down
		//if so, move!
		if(InputIsDown(INP_UP))
		{
			if(CheckFlag(TATA_FLAG_RUN)
				&& tataOBJState != m_glideState)
				MoveDir(g_runScale);
			else
				MoveDir(1);
		}
		else if(InputIsDown(INP_DOWN))
		{
			MoveDir(-1);
			SetFlag(TATA_FLAG_BACKWALK, true);
		}
	}
}

void ParaTaTa::InputStrafe()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK)
		|| (tataOBJState == CREANIM_FALL)
		|| (tataOBJState == m_glideState))
	{
		if(InputIsDown(INP_LEFT))
			MoveStrafe(-0.5f);
		else if(InputIsDown(INP_RIGHT))
			MoveStrafe(0.5f);
	}
}

void ParaTaTa::MoveAnim(eMoveType type)
{
	hOBJ tataOBJ = GetOBJ(); assert(tataOBJ);

	int OBJState = OBJGetCurState(tataOBJ);

	if((OBJState >= CREANIM_IDLE1 && OBJState <= CREANIM_IDLE_MAX)
		|| OBJState == CREANIM_WALK
		|| OBJState == CREANIM_BACKWALK
		|| OBJState == CREANIM_FALL
		|| OBJState == m_glideState)
	{
		switch(type)
		{
		case MOVE_FALL:
			if(!m_bGlide)
				OBJSetState(tataOBJ, CREANIM_FALL);
			else
				OBJSetState(tataOBJ, m_glideState);

			break;

		case MOVE_NONE:
			{
				int newIdleState = Random(CREANIM_IDLE1, CREANIM_IDLE_MAX);

				if(OBJState >= CREANIM_IDLE1 && OBJState <= CREANIM_IDLE_MAX)
				{
					if(OBJIsStateEnd(tataOBJ))
						OBJSetState(tataOBJ, newIdleState);
				}
				else
					OBJSetState(tataOBJ, CREANIM_IDLE1);
			}
			break;
		
		case MOVE_WALK:
			if(CheckFlag(TATA_FLAG_BACKWALK))
			{
				OBJSetState(tataOBJ, CREANIM_BACKWALK);
				SetFlag(TATA_FLAG_BACKWALK, false);
			}
			else
				OBJSetState(tataOBJ, CREANIM_WALK);
			break;
		}
	}
	else if(OBJState == CREANIM_OUCH && OBJIsStateEnd(tataOBJ))
		OBJSetState(tataOBJ, CREANIM_IDLE1);
}