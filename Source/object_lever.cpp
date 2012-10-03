#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Lever::Lever() : Object(OBJECT_LEVER), m_onScript(0), m_offScript(0), 
m_onLoc(0,0,0), m_offLoc(0,0,0), m_bLastOn(false), m_bMoving(false), m_bOnce(false)
{
	//we do not want buttons to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);
}

Lever::~Lever()
{
	RemoveScript(true);
	RemoveScript(false);
}

int Lever::Callback(unsigned int msg, unsigned int wParam, int lParam)
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
			if(m_bMoving && !m_bOnce)
			{
				D3DXVECTOR3 zeroVel(0,0,0);

				//pressed by something...
				if(bIsOn)
				{
					//move until we reach the 'on' location
					SetVel(m_onLoc - GetLoc());
					
					//we are done
					if(GetVel() == zeroVel)
					{
						execScript = m_onScript;

						m_bMoving = false;

						if(CheckFlag(OBJ_FLAG_ONCE_ONLY))
							m_bOnce = true;
					}
				}
				//nobody pressing.
				else
				{
					//move until we reach the 'off' location
					SetVel(m_offLoc - GetLoc());
					
					//we are close enough
					//activate the script for 'off'
					if(GetVel() == zeroVel)
					{
						execScript = m_offScript;

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
				ScriptReset(execScript);

				RETCODE ret = RETCODE_SUCCESS;
				
				while(ret != RETCODE_BREAK)
					ret = ScriptUpdate(&GetID(), execScript);
			}
		}
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(pEntity && !m_bMoving)
			{
				//only 'use melee' projectiles can push us!
				if((pEntity->GetEntityType() == ENTITY_TYPE_PROJECTILE
					&& pEntity->GetSubType() == PROJ_USEMELEE))
				{
					//play 'click' sound
					BASS_3DVECTOR pos; memcpy(&pos, (float*)GetLoc(), sizeof(pos)); pos.z *= -1;
					TaTaSoundPlay(7, &pos, 0, 0);

					SetFlag(OBJ_FLAG_ON, !CheckFlag(OBJ_FLAG_ON));
					m_bMoving = true;
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
RETCODE Lever::LoadScript(bool bOn, const char *filename, const char *section)
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
void Lever::RemoveScript(bool bOn)
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