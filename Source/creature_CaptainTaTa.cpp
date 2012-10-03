#include "tata_main.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

#include "tata_globals.h"

//Captain Ta-Ta interface
CaptainTaTa::~CaptainTaTa()
{
	//nothing...
}

//input functions
void CaptainTaTa::InputRotate()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	if((tataOBJState == m_blockStandState) 
		|| (tataOBJState == m_blockMoveState))
	{
		if(InputIsDown(INP_LEFT))
			Rotate(0.5);
		else if(InputIsDown(INP_RIGHT))
			Rotate(-0.5);
	}
	else
	{
		if(InputIsDown(INP_LEFT))
			Rotate(1);
		else if(InputIsDown(INP_RIGHT))
			Rotate(-1);
	}
}

void CaptainTaTa::InputStrafe()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK)
		|| (tataOBJState == CREANIM_FALL)
		|| (tataOBJState == m_blockStandState) 
		|| (tataOBJState == m_blockMoveState))
	{
		if(InputIsDown(INP_LEFT))
			MoveStrafe(-0.5f);
		else if(InputIsDown(INP_RIGHT))
			MoveStrafe(0.5f);
	}
}

void CaptainTaTa::InputWalk()
{
	int tataOBJState = OBJGetCurState(GetOBJ());

	//we can only move and perform
	//action if we are idle or 
	//walking/backwalking
	if((tataOBJState >= CREANIM_IDLE1 && tataOBJState <= CREANIM_IDLE_MAX)
		|| (tataOBJState == CREANIM_WALK)
		|| (tataOBJState == CREANIM_BACKWALK)
		|| (tataOBJState == m_blockStandState) 
		|| (tataOBJState == m_blockMoveState))
	{
		//check to see if the 'up'/'down' button is down
		//if so, move!
		if(InputIsDown(INP_UP))
		{
			if(CheckFlag(TATA_FLAG_RUN)
				&& !(tataOBJState == m_blockStandState || tataOBJState == m_blockMoveState))
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

void CaptainTaTa::MoveAnim(eMoveType type)
{
	hOBJ tataOBJ = GetOBJ(); assert(tataOBJ);

	int OBJState = OBJGetCurState(tataOBJ);

	if((OBJState >= CREANIM_IDLE1 && OBJState <= CREANIM_IDLE_MAX)
		|| OBJState == CREANIM_WALK
		|| OBJState == CREANIM_BACKWALK
		|| OBJState == CREANIM_FALL)
	{
		switch(type)
		{
		case MOVE_FALL:
			OBJSetState(tataOBJ, CREANIM_FALL);
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
	else if((OBJState == m_blockStandState) 
		|| (OBJState == m_blockMoveState))
	{
		switch(type)
		{
		case MOVE_NONE:
			OBJSetState(m_obj, m_blockStandState);
			break;
		case MOVE_WALK:
			OBJSetState(tataOBJ, m_blockMoveState);
			SetFlag(TATA_FLAG_BACKWALK, false);
			break;
		}
	}
	else if(OBJState == CREANIM_OUCH && OBJIsStateEnd(tataOBJ))
		OBJSetState(tataOBJ, CREANIM_IDLE1);
}

int CaptainTaTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_blockState = CfgGetItemInt(cfg, "special", "blockState");

			if(m_blockState == -1)
				m_blockState = 0;

			m_blockStandState = CfgGetItemInt(cfg, "special", "blockStandState");

			if(m_blockStandState == -1)
				m_blockStandState = 0;

			m_blockMoveState = CfgGetItemInt(cfg, "special", "blockMoveState");

			if(m_blockMoveState == -1)
				m_blockMoveState = 0;

			m_blockSpd = CfgGetItemFloat(cfg, "special", "blockMoveSpd");
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState == m_blockState && OBJIsStateEnd(m_obj))
			{
				OBJSetState(m_obj, m_blockStandState);
			}
			else if((OBJState != m_blockState
				     && OBJState != m_blockStandState
				     && OBJState != m_blockMoveState)
				&& m_blockSpd == GetMoveSpd())
			{
				//revert back to normal speed
				SetMoveSpd(m_normSpd);
			}
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_RELEASED)
		{
			int OBJState = OBJGetCurState(m_obj);

			if(OBJState != m_blockState
				&& OBJState != m_blockStandState
				&& OBJState != m_blockMoveState)
			{
				//prepare blocking
				OBJSetState(m_obj, m_blockState);

				//set to slow block spd
				m_normSpd = GetMoveSpd();

				SetMoveSpd(m_blockSpd);

				//play shield 'on' sound
				CREPlaySound(0);
			}
			else
			{
				OBJSetState(m_obj, CREANIM_IDLE1);

				//revert back to normal speed
				SetMoveSpd(m_normSpd);

				//play shield 'off' sound
				CREPlaySound(1);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			
			int OBJState = OBJGetCurState(m_obj);

			//check to see if it is a spit and that we are defending.
			if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE
				&& (OBJState == m_blockState
					|| OBJState == m_blockStandState
					|| OBJState == m_blockMoveState))
			{
				Projectile *pProj = (Projectile *)pEntity;

				float nd = D3DXVec3Dot(&GetDir(), &pProj->GetDir());

				//make sure we are facing the projectile
				if(nd < 0)
				{
					//reflect the projectile, if it's a spit
					if(pEntity->GetSubType() == PROJ_SPIT)
					{
						//change the projectile's owner 
						pProj->SetOwner(GetID());

						//reflect the projectile
						D3DXVECTOR3 refl;

						refl = pProj->GetDir() - (2*nd*GetDir());

						pProj->SetDir(refl);
					}
					else
					{
						EntityCommon *pEntityOwner = (EntityCommon *)IDPageQuery(pProj->GetOwner());
						if(pEntityOwner)
						{
							hOBJ obj = pEntityOwner->GetOBJ();
							if(obj)
								OBJSetState(obj, 0);
						}

						//set to no owner
						Id invID = {0,-1};
						pProj->SetOwner(invID);
					}
				}
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}