#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_tata.h"

#include "tata_projectiles_common.h"

KeyKeyTa::KeyKeyTa() : TaTaCommon(TATA_KEYKEYTA), m_useState(0), 
m_climbState(0), m_bClimb(false), m_bCeiling(false), m_bCanClimb(false),
m_climbSnd(0)
{
}

KeyKeyTa::~KeyKeyTa()
{
	if(m_climbSnd)
		BASS_ChannelStop(m_climbSnd);
}

int KeyKeyTa::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_useState = CfgGetItemInt(cfg, "special", "useState");

			if(m_useState == -1)
				m_useState = 0;

			m_climbState = CfgGetItemInt(cfg, "special", "climbState");

			if(m_climbState == -1)
				m_climbState = 0;

			m_climbSpd.MoveSetSpeed(CfgGetItemFloat(cfg, "special", "climbSpd"));
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int objState = OBJGetCurState(GetOBJ());

			if(m_climbSnd)
			{
				if(objState != m_climbState)
				{ BASS_ChannelStop(m_climbSnd); m_climbSnd = 0; }
				else
				{
					BASS_3DVECTOR pos, orient;

					memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
					memcpy(&orient, (float*)GetDir(), sizeof(orient)); orient.z *= -1;

					//update the position
					BASS_ChannelSet3DPosition(m_climbSnd, &pos, &orient, 0);
				}
			}

			//turn to semi transparent if standby
			//also be ignored by enemies
			if(GetStatus() == CRESTATUS_NORMAL)
			{
				if(objState >= CREANIM_IDLE1 && objState <= CREANIM_IDLE_MAX
					&& !CheckFlag(TATA_FLAG_ENEMY_IGNORE))
				{
					SetFlag(TATA_FLAG_ENEMY_IGNORE, true);
					OBJSetAlpha(GetOBJ(), 0.8f);
					OBJSetToLast(GetOBJ());
				}
				else if((objState < CREANIM_IDLE1 || objState > CREANIM_IDLE_MAX)
					&& CheckFlag(TATA_FLAG_ENEMY_IGNORE))
				{
					SetFlag(TATA_FLAG_ENEMY_IGNORE, false);
					OBJSetAlpha(GetOBJ(), 1.0f);
				}
			}
			else if(CheckFlag(TATA_FLAG_ENEMY_IGNORE))
				SetFlag(TATA_FLAG_ENEMY_IGNORE, false);

			if(m_bClimb) //let's climb
			{
				SetFlag(TATA_FLAG_DISABLE_ROTATE, true);

				SetLoc(GetLoc());
				
				D3DXVECTOR3 vel = GetVel();

				vel.y = m_climbSpd.MoveUpdate(g_timeElapse);

				SetVel(vel);

				m_bClimb = false;

				if(InputIsDown(INP_UP)
					&& GAME_IDISEQUAL(g_world->TataGetCurrent(), GetID()))
				{
					MoveDir(1);
				}
				else
					MoveDir(-1);
			}
			else if(!m_bClimb
				&& objState == m_climbState)
			{
				SetFlag(TATA_FLAG_DISABLE_ROTATE, false);
				OBJSetState(GetOBJ(), CREANIM_IDLE1);
			}
			else if(objState == m_useState && OBJIsStateEnd(m_obj))
			{
				OBJSetState(m_obj, CREANIM_IDLE1);
			}

			m_bCeiling = false;
		}
		break;

	case CREMSG_ACTION:
		if(wParam == INP_STATE_DOWN)
		{
			//MoveDir(1);
			m_bCanClimb = true;
		}
		else if(wParam == INP_STATE_RELEASED)
		{
			m_bCanClimb = false;

			/*if(!InputIsDown(INP_UP))
			{
				if(!m_bClimb
					&& OBJGetCurState(GetOBJ()) != m_climbState)
				{
					//unlock doors
					new OpenMelee(GetID());

					OBJSetState(GetOBJ(), m_useState);
				}
			}*/
		}

		break;

	case ENTITYMSG_WORLDCOLLIDE: //wParam = hQBSP, lParam = gfxTrace *
		{
			hQBSP qbsp = (hQBSP)wParam;
			gfxTrace *pTrace = (gfxTrace *)lParam;

			//must be a straight wall
			//and that it has a texture
			if(!m_bCeiling && m_bCanClimb
				&& pTrace->norm[eY] == 0
				&& (pTrace->norm[eX] != 0
				|| pTrace->norm[eZ] != 0)
				&& QBSPGetTexture(qbsp, pTrace->txtInd))
			{
				if(OBJGetCurState(GetOBJ()) != m_climbState)
				{
					D3DXVECTOR3 dir(-pTrace->norm[eX], 0, -pTrace->norm[eZ]);
					SetDir(dir);
				}

				m_bClimb = true;
				
				OBJSetState(GetOBJ(), m_climbState);

				//play climbing sound
				if(!m_climbSnd)
					m_climbSnd = CREPlaySound(14);
			}
			else if(pTrace->norm[eY] < 0)
			{
				m_bClimb = false;
				m_bCeiling = true;
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;
			
			int OBJState = OBJGetCurState(m_obj);

			if(pEntity->GetEntityType() == ENTITY_TYPE_ENEMY
				&& !pEntity->CheckFlag(CRE_FLAG_TAG)
				&& CheckFlag(TATA_FLAG_ENEMY_IGNORE))
			{
				Creature *pCre = (Creature *)pEntity;
				if(pCre->GetWayPoint())
				{
					pCre->GetWayPoint()->SetToPrev();
					pCre->GetWayPoint()->Reverse(!pCre->GetWayPoint()->IsReverse());
					pCre->GetWayPoint()->SetNext();
					pCre->SetFlag(CRE_FLAG_TAG, true);
				}
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}

void KeyKeyTa::OpenTheDoor()
{
	//that's it...
	OBJSetState(GetOBJ(), m_useState);

	//play door open sound
	CREPlaySound(15);
}