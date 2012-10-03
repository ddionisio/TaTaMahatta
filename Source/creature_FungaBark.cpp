#include "tata_main.h"

#include "tata_globals.h"

#include "tata_creature_enemy.h"

#include "tata_projectiles_common.h"

//animation
typedef enum {
	FUNGABARK_ANIM_IDLE1,
	FUNGABARK_ANIM_IDLE2,
	FUNGABARK_ANIM_IDLE3,
	FUNGABARK_ANIM_IDLE_MAX = FUNGABARK_ANIM_IDLE3,
	FUNGABARK_ANIM_OUCH,
	FUNGABARK_ANIM_DEATH,
	FUNGABARK_ANIM_PREP_ATTACK,
	FUNGABARK_ANIM_ATTACK
} eFungaBarkAnim;

FungaBark::FungaBark() : EnemyCommon(ENEMY_FUNGABARK), 
m_attackRadiusSQ(0), m_attackBound(0,0,0), m_growlSnd(0)
{
}

FungaBark::~FungaBark()
{
}

int FungaBark::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_CFGLOAD:
		{
			hCFG cfg = (hCFG)wParam;

			m_attackRadiusSQ = CfgGetItemFloat(cfg, "special", "attackRange");

			m_attackRadiusSQ *= m_attackRadiusSQ;

			//get the attack bound m_attackBound
			m_attackBound.x = CfgGetItemFloat(cfg, "special", "attackBoundX");
			m_attackBound.y = CfgGetItemFloat(cfg, "special", "attackBoundY");
			m_attackBound.z = CfgGetItemFloat(cfg, "special", "attackBoundZ");
		}
		break;

	case ENTITYMSG_UPDATE:
		{
			int state = OBJGetCurState(GetOBJ());

			if(state != FUNGABARK_ANIM_ATTACK
				&& state != FUNGABARK_ANIM_OUCH)
			{
				D3DXVECTOR3 vec;
				float vecLenSq;
				Creature *pCre;
				//check for Ta-Tas nearby
				for(int i = 0; i < MAXTATAWORLD; i++)
				{
					pCre = (Creature *)IDPageQuery(g_world->TataGet(i));

					if(pCre
						&& !pCre->CheckFlag(TATA_FLAG_ENEMY_IGNORE))
					{
						vec = pCre->GetLoc() - GetLoc();
						vecLenSq = D3DXVec3LengthSq(&vec);

						//uh oh, better run!
						if(vecLenSq <= m_attackRadiusSQ)
						{
							//face the Ta-Ta
							OBJSetOrientation(GetOBJ(), (float*)vec);

							//prepare to attack
							OBJSetState(GetOBJ(), FUNGABARK_ANIM_PREP_ATTACK);

							//growl!
							if(!m_growlSnd)
								m_growlSnd = CREPlaySound(16);

							break;
						}
					}
				}
			}
			
			if(OBJIsStateEnd(GetOBJ()))
			{
				//done preparing?
				if(state == FUNGABARK_ANIM_PREP_ATTACK)
				{
					//set to attack
					OBJSetState(GetOBJ(), FUNGABARK_ANIM_ATTACK);

					//create the fungal bark super special bite projectile
					new AttackMelee(GetID(), m_attackBound, OBJJointGetIndex(m_obj, "tip_b"));

					m_growlSnd = 0;

					//BARK!
					CREPlaySound(17);
				}
				else
				{
					//random idle
					int newIdleState = Random(FUNGABARK_ANIM_IDLE1, FUNGABARK_ANIM_IDLE_MAX);

					OBJSetState(GetOBJ(), newIdleState);
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		break;

	case ENTITYMSG_REQUESTSOUND:
		{
			switch(wParam)
			{
			case SND_REQ_PROJ_HIT_WALL:
				*((int*)lParam) = 4;
				break;

			case SND_REQ_PROJ_HIT_CRE:
				*((int*)lParam) = 18;
				break;
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}

int FungaBark::GetDeathState() 
{ 
	return FUNGABARK_ANIM_DEATH; 
}

int FungaBark::GetOuchState() 
{ 
	return FUNGABARK_ANIM_OUCH; 
}