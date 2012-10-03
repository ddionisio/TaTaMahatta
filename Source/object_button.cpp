#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

//here for now...
#define BTN_SPD		100

Button::Button() : Object(OBJECT_BUTTON), m_onScript(0), m_offScript(0), 
m_onLoc(0,0,0), m_offLoc(0,0,0), m_bLastOn(false), m_bMoving(false), m_bOnce(false),
m_bProjPush(false)
{
	//we do not want buttons to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	m_moveSpd.MoveSetSpeed(BTN_SPD);
}

Button::~Button()
{
	RemoveScript(true);
	RemoveScript(false);
}

int Button::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		{
			hSCRIPT execScript = 0;

			bool bIsOn = CheckFlag(OBJ_FLAG_ON);

			if(m_bLastOn && !bIsOn)
				m_bMoving = true;

			//if state changed, then determine how we will move
			if(m_bLastOn != bIsOn && m_bMoving && !m_bOnce)
			{
				D3DXVECTOR3 dir, zeroVel(0,0,0);
				float len;
				float spd = m_moveSpd.MoveUpdate(g_timeElapse);

				//pressed by something...
				if(bIsOn)
				{
					//move until we reach the 'on' location
					dir = m_onLoc - GetLoc();
					len = D3DXVec3Length(&dir);
					
					if(len > spd)
					{
						dir /= len;

						SetDir(dir);

						SetVel(GetVel() + (spd*dir));
					}
					//we are close enough
					//activate the script for 'on'
					else
					{
						SetVel(dir);

						execScript = m_onScript;
						m_bLastOn = true;

						m_bMoving = false;

						m_bProjPush = false;

						if(CheckFlag(OBJ_FLAG_ONCE_ONLY))
							m_bOnce = true;
					}
				}
				//nobody pressing.
				else
				{
					//move until we reach the 'off' location
					dir = m_offLoc - GetLoc();
					len = D3DXVec3Length(&dir);
					
					if(len > spd)
					{
						dir /= len;

						SetDir(dir);

						SetVel(GetVel() + (spd*dir));
					}
					//we are close enough
					//activate the script for 'off'
					else
					{
						SetVel(dir);

						execScript = m_offScript;
						m_bLastOn = false;

						m_bMoving = false;

						if(CheckFlag(OBJ_FLAG_ONCE_ONLY))
							m_bOnce = true;
					}
				}
			}

			//Execute the script until it is done
			//THERE BETTER NOT BE ANY INFINITE WHILE LOOP INSIDE THE
			//SCRIPT!!!
			if(execScript)
			{
				//button down sound
				BASS_3DVECTOR pos; memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
				TaTaSoundPlay(34, &pos, 0, 0);

				ScriptReset(execScript);

				RETCODE ret = RETCODE_SUCCESS;
				
				while(ret != RETCODE_BREAK)
					ret = ScriptUpdate(&GetID(), execScript);
			}

			if(!m_bProjPush)
				SetFlag(OBJ_FLAG_ON, false);
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity && !CheckFlag(OBJ_FLAG_ON))
			{
				//if it is a creature and that we allow creatures
				//to push us...
				if((pEntity->GetEntityType() == ENTITY_TYPE_TATA
					|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY
					|| (pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE 
					    && pEntity->CheckFlag(PROJ_FLAG_PUSHBUTTON)))
					&& CheckFlag(OBJ_FLAG_PUSHABLE))
				{
					SetFlag(OBJ_FLAG_ON, true);
					m_bMoving = true;

					//must've been a bullet, spit or something...
					if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
						m_bProjPush = true;
				}
				else if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
					|| pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE)
				{
					//must be a block
					if(pEntity->GetSubType() == OBJECT_BLOCK)
					{
						SetFlag(OBJ_FLAG_ON, true);
						m_bMoving = true;
					}
					//must've been a bullet, spit or something...
					else if(pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE
						&& pEntity->CheckFlag(PROJ_FLAG_PUSHBUTTON))
					{
						SetFlag(OBJ_FLAG_ON, true);
						m_bMoving = true;
						m_bProjPush = true;
					}
				}
			}
		}
		break;

	case ENTITYMSG_ALLOWGRAVITY:
		//simply don't allow gravity
		return 0;
	}

	return RETCODE_SUCCESS;
}

// if section is NULL, then
// the entire file is parsed.
// format of section: [<section>_begin]
// 				      ... // code
//					  [<section>_end]
//bOn == true  if the script is for 'on'
//bOn == false if the script is for 'off'
RETCODE Button::LoadScript(bool bOn, const char *filename, const char *section)
{
	RemoveScript(bOn);

	char beginStr[MAXCHARBUFF], endStr[MAXCHARBUFF];

	strcpy(beginStr, "[");
	strcat(beginStr, section);
	strcat(beginStr, "_begin]");

	strcpy(endStr, "[");
	strcat(endStr, section);
	strcat(endStr, "_end]");

	if(bOn)
		m_onScript  = ScriptLoad(&GetID(), filename, beginStr, endStr);
	else
		m_offScript = ScriptLoad(&GetID(), filename, beginStr, endStr);

	return RETCODE_SUCCESS;
}

//destroys current script
//bOn == true  if the script is for 'on'
//bOn == false if the script is for 'off'
void Button::RemoveScript(bool bOn)
{
	if(bOn && m_onScript)
	{
		ScriptDestroy(&GetID(), &m_onScript);

		m_onScript = 0;
	}
	else if(m_offScript)
	{
		ScriptDestroy(&GetID(), &m_offScript);

		m_offScript = 0;
	}
}