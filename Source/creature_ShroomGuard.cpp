#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

/*
Walk - think walking mushroom with feet and you get the idea.
	Ouch - imagine beating up a life size mushroom.
	Death - what a mushroom looks like after you step on it.
	Attack - imagine getting slapped by a mushroom.

*/

//animation
typedef enum {
	SHROOMGUARD_ANIM_IDLE,
	SHROOMGUARD_ANIM_WALK,
	SHROOMGUARD_ANIM_OUCH,
	SHROOMGUARD_ANIM_DEATH,
	SHROOMGUARD_ANIM_ATTACK_PREP,
	SHROOMGUARD_ANIM_ATTACK
} eShroomGuardAnim;

ShroomGuard::ShroomGuard() : EnemyCommon(ENEMY_SHROOMGUARD),
m_attackRadiusSQ(0), m_followRadiusSQ(0), m_attackBound(0,0,0),
m_followSnd(0)
{
}

ShroomGuard::~ShroomGuard()
{
	if(m_followSnd)
		BASS_ChannelStop(m_followSnd);
}

int ShroomGuard::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_attackRadiusSQ = CfgGetItemFloat(cfg, "special", "attackRange");
			m_attackRadiusSQ *= m_attackRadiusSQ;

			m_followRadiusSQ = CfgGetItemFloat(cfg, "special", "followRange");
			m_followRadiusSQ *= m_followRadiusSQ;

			//get the attack bound m_attackBound
			m_attackBound.x = CfgGetItemFloat(cfg, "special", "attackBoundX");
			m_attackBound.y = CfgGetItemFloat(cfg, "special", "attackBoundY");
			m_attackBound.z = CfgGetItemFloat(cfg, "special", "attackBoundZ");
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			//if we are attacking or hurt...
			if(OBJGetCurState(GetOBJ()) == SHROOMGUARD_ANIM_ATTACK
				|| OBJGetCurState(GetOBJ()) == SHROOMGUARD_ANIM_OUCH)
			{
				//face to the same dir.
				SetDir(m_attackDir);

				//state is done, so get back to walking
				if(OBJIsStateEnd(GetOBJ()))
				{
					OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_WALK);
				}
				else
				{
					//don't move
					D3DXVECTOR3 vel =GetVel();
					vel.x = vel.z = 0;
					SetVel(vel);
				}
			}
			else
			{
				D3DXVECTOR3 vec, vecMin;
				float vecLenSq, vecLenMin = m_followRadiusSQ;
				Creature *pCre, *pCreMin=0;
				
				for(int i = 0; i < MAXTATAWORLD; i++)
				{
					pCre = (Creature *)IDPageQuery(g_world->TataGet(i));

					if(pCre
						&& !pCre->CheckFlag(TATA_FLAG_ENEMY_IGNORE))
					{
						vec = pCre->GetLoc() - GetLoc();
						vecLenSq = D3DXVec3LengthSq(&vec);

						//uh oh, better run!
						//check to be sure that the player is 
						//within height range
						if(vecLenSq <= vecLenMin
							&& pCre->GetLoc().y >= (GetLoc().y + BoundGetMin().y + pCre->BoundGetMin().y)
							&& pCre->GetLoc().y <= (GetLoc().y + BoundGetMax().y + pCre->BoundGetMax().y))
						{
							vecMin    = vec;
							vecLenMin = vecLenSq;
							pCreMin   = pCre;
						}
					}
				}

				//did we find anyone near?
				if(pCreMin)
				{
					float len = sqrtf(vecLenMin);
					vecMin /= len;
					vecMin.y = 0;

					//face the Ta-Ta
					SetDir(vecMin);
					m_attackDir = vecMin;

					//preparing attack, don't move
					if(OBJGetCurState(GetOBJ()) == SHROOMGUARD_ANIM_ATTACK_PREP)
					{
						//don't move
						D3DXVECTOR3 vel = GetVel();
						vel.x = vel.z = 0;
						SetVel(vel);
					}
					//attack Ta-Ta if too close
					else if(vecLenMin <= m_attackRadiusSQ)
					{
						//set to attack preparation
						OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_ATTACK_PREP);
					}
					//follow the Ta-Ta
					else
					{
						D3DXVECTOR3 vel = GetVel();
						vel.x = vel.z = 0;
						SetVel(vel);

						MoveDir(1);
					}

					//play frontin' sound
					if(!m_followSnd)
						m_followSnd = CREPlaySound(22);
				}
				else if(m_followSnd)
				{ BASS_ChannelStop(m_followSnd); m_followSnd = 0; }

				//are we done preparing attack?
				if(OBJGetCurState(GetOBJ()) == SHROOMGUARD_ANIM_ATTACK_PREP
					&& OBJIsStateEnd(GetOBJ()))
				{
					//set to attack
					OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_ATTACK);

					//play swing melee sound
					CREPlaySound(3);

					//create the ultimate bushido baka supra move
					new AttackMelee2(GetID(), m_attackBound, 
						OBJJointGetIndex(m_obj, "weapon_begin"),
						OBJJointGetIndex(m_obj, "weapon_end"));
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
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

int ShroomGuard::GetDeathState()
{
	return SHROOMGUARD_ANIM_DEATH;
}

int ShroomGuard::GetOuchState() 
{ 
	return SHROOMGUARD_ANIM_OUCH; 
}

void ShroomGuard::MoveAnim(eMoveType type)
{
	int OBJState = OBJGetCurState(GetOBJ());

	if(OBJState == SHROOMGUARD_ANIM_IDLE
		|| OBJState == SHROOMGUARD_ANIM_WALK)
	{
		switch(type)
		{
		case MOVE_FALL:
			OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_WALK);
			break;

		case MOVE_NONE:
			OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_IDLE);
			break;
		
		case MOVE_WALK:
			OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_WALK);
			break;
		}
	}
	else if(OBJState == SHROOMGUARD_ANIM_OUCH && OBJIsStateEnd(GetOBJ()))
		OBJSetState(GetOBJ(), SHROOMGUARD_ANIM_IDLE);
}