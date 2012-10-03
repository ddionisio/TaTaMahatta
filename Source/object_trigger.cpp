#include "tata_main.h"

#include "tata_object_common.h"

#include "tata_globals.h"

Trigger::Trigger() : Object(OBJECT_TRIGGER), m_onScript(0), m_bCollide(false),
m_bOnce(false), m_allowMultiple(false)
{
	m_collideWith.counter = -1;

	//we do not want triggers to collide with world
	SetFlag(ENTITY_FLAG_NOWORLDCOLLISION, true);

	SetFlag(ENTITY_FLAG_NONSOLID, true);
}

Trigger::~Trigger()
{
	RemoveScript();
}

RETCODE Trigger::LoadScript(const char *filename, const char *section)
{
	RemoveScript();

	char beginStr[MAXCHARBUFF], endStr[MAXCHARBUFF];

	strcpy(beginStr, "[");
	strcat(beginStr, section);
	strcat(beginStr, "_begin]");

	strcpy(endStr, "[");
	strcat(endStr, section);
	strcat(endStr, "_end]");

	m_onScript = ScriptLoad(&GetID(), filename, beginStr, endStr);

	return RETCODE_SUCCESS;
}

void Trigger::RemoveScript()
{
	if(m_onScript)
	{
		ScriptDestroy(&GetID(), &m_onScript);
		m_onScript = 0;
	}
}

Id Trigger::GetCollideWith()
{
	return m_collideWith;
}

void Trigger::AllowMultipleEntities(bool bYes)
{
	m_allowMultiple = bYes;
}

int Trigger::Callback(unsigned int msg, unsigned int wParam, int lParam)
{
	switch(msg)
	{
	case ENTITYMSG_UPDATE:
		break;

	case ENTITYMSG_ENTITYCOLLIDE:
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(!m_bOnce)
			{
				if(m_allowMultiple)
				{
					m_collideWith = pEntity->GetID();

					//execute script
					ScriptReset(m_onScript);

					RETCODE ret = RETCODE_SUCCESS;
					
					while(ret != RETCODE_BREAK)
						ret = ScriptUpdate(&GetID(), m_onScript);

					if(CheckFlag(OBJ_FLAG_ONCE_ONLY))
						m_bOnce = true;
				}
				else if(!m_bCollide)
				{
					if(!GAME_IDISEQUAL(m_collideWith, pEntity->GetID()))
					{
						m_collideWith = pEntity->GetID();
						m_bCollide = true;

						//execute script
						ScriptReset(m_onScript);

						RETCODE ret = RETCODE_SUCCESS;
						
						while(ret != RETCODE_BREAK)
							ret = ScriptUpdate(&GetID(), m_onScript);

						if(CheckFlag(OBJ_FLAG_ONCE_ONLY))
							m_bOnce = true;
					}
				}
			}
		}
		break;

	case ENTITYMSG_ENTITYNOCOLLIDE:
		if(!m_allowMultiple)
		{
			EntityCommon *pEntity = (EntityCommon *)wParam;

			if(m_bCollide)
			{
				if(GAME_IDISEQUAL(m_collideWith, pEntity->GetID()))
				{
					m_bCollide = false;
					m_collideWith.counter = -1;
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