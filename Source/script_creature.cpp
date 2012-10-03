#include "tata_main.h"

#include "tata_world.h"

#include "tata_script.h"

#include "tata_globals.h"

#include "tata_collision.h"

//CreatureHit (string name, int amt)
//damage creature with given amount
PROTECTED long S_CreatureHit(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			char *str;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				str = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				str = (char *)ScriptParamGetVar(param, 0);

			int amt = *((int *)ScriptParamGetVar(param, 1));

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(str));

			if(pEntity
				&& (pEntity->GetEntityType() == ENTITY_TYPE_TATA
				     || pEntity->GetEntityType() == ENTITY_TYPE_ENEMY))
			{
				Creature *pCre = (Creature *)pEntity;

				pCre->Hit(amt);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//CreatureSetHitPoint (string name, int amt)
//set creature's hit point
PROTECTED long S_CreatureSetHitPoint(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			char *str;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				str = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				str = (char *)ScriptParamGetVar(param, 0);

			int amt = *((int *)ScriptParamGetVar(param, 1));

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(str));

			if(pEntity
				&& (pEntity->GetEntityType() == ENTITY_TYPE_TATA
				     || pEntity->GetEntityType() == ENTITY_TYPE_ENEMY))
			{
				Creature *pCre = (Creature *)pEntity;

				pCre->SetCurHit(amt);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//CreatureTeleport (string name, string targetName)
//teleport given creature to target
PROTECTED long S_CreatureTeleport(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			char *creName, *tgtName;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				creName = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				creName = (char *)ScriptParamGetVar(param, 0);

			if(ScriptParamGetType(param, 1) == eVarPtr)
				tgtName = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				tgtName = (char *)ScriptParamGetVar(param, 1);

			EntityCommon *pEntity = (EntityCommon*)IDPageQuery(CollisionGetEntity(creName));

			if(pEntity)
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
				|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
				{
					Creature *pCre = (Creature *)pEntity;
					pCre->Teleport(tgtName);
				}
				else if(g_world)
				{
					D3DXVECTOR3 loc;
					if(g_world->TargetGet(tgtName, loc))
						pEntity->SetLoc(loc);
				}
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}