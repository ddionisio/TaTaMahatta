#include "tata_main.h"

#include "tata_world.h"

#include "tata_creature.h"

#include "tata_object_common.h"

#include "tata_script.h"

#include "tata_globals.h"

#include "tata_collision.h"

//EntityID (string entityname)
PROTECTED long S_EntityIDCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	Id *pID = (Id *)ScriptVarGetData(var);

	switch(msg)
	{
	case VAR_CREATE:
		pID = new Id; assert(pID);

		pID->ID = 0;
		pID->counter = -1;

		ScriptVarSetData(var, pID);
		break;

	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			char *str;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				str = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				str = (char *)ScriptParamGetVar(param, 0);

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(str));

			if(pEntity)
				memcpy(pID, &pEntity->GetID(), sizeof(Id));
		}
		break;

	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			pOut->paramVarType = eVarInt;

			pOut->dat._int = GAME_IDVALID(*pID);
		}
		break;

	case VAR_DESTROY:
		delete pID;
		break;
	}

	return RETCODE_SUCCESS;
}

//EntityAvailable (string name, int iOut)
//check to see if an entity by the given name exists, iOut = 1 if found,
//0 otherwise.
PROTECTED long S_EntityAvailable(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 1);

			if(GAME_IDVALID(CollisionGetEntity(str))
				|| GAME_IDVALID(CollisionGetEntityDoodad(str)))
				*iOut = 1;
			else
				*iOut = 0;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntityGetTypeName (string name, string typeOut)
//get the type of given entity
PROTECTED long S_EntityGetTypeName(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr;
			string *sOut;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			sOut = (string *)ScriptParamGetVar(param, 1);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity)
			{
				(*sOut) = pEntity->GetEntityTypeStr();
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntityGetSubTypeName (string name, string typeOut)
//get the sub-type of given entity
PROTECTED long S_EntityGetSubTypeName(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr;
			string *sOut;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			sOut = (string *)ScriptParamGetVar(param, 1);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity)
			{
				(*sOut) = pEntity->GetSubTypeStr();
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntityPollDeath (string name)
//kill entity
PROTECTED long S_EntityPollDeath(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(pEntity)
			{
				pEntity->SetFlag(ENTITY_FLAG_POLLDEATH, true);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntitySetWP (string name, string wpName, string mode, int bBackward)
//Set entity waypoint
PROTECTED long S_EntitySetWP(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr, *wpStr, *wpModeStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			if(ScriptParamGetType(param, 1) == eVarPtr)
				wpStr = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				wpStr = (char *)ScriptParamGetVar(param, 1);

			if(ScriptParamGetType(param, 2) == eVarPtr)
				wpModeStr = (char *)((string *)ScriptParamGetVar(param, 2))->c_str();
			else
				wpModeStr = (char *)ScriptParamGetVar(param, 2);

			int bBackward = *(int *)ScriptParamGetVar(param, 3);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			WayptNode *pWayPt=0;

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			//TODO: set WP to be part of Entity...
			if(pEntity && g_world)
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
					|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
				{
					Creature *pCre = (Creature*)pEntity;
					pCre->SetWayPt(g_world->WaypointGet(wpStr));
					pWayPt = pCre->GetWayPoint();
				}
				else if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT)
				{
					if(pEntity->GetSubType() == OBJECT_PLATFORM)
					{
						Platform *pPlatform = (Platform *)pEntity;
						pPlatform->SetWayPt(g_world->WaypointGet(wpStr));
						pWayPt = pPlatform->GetWayPoint();
					}
					else if(pEntity->GetSubType() == OBJECT_DOODAD)
					{
						Doodad *pDoodad = (Doodad*)pEntity;
						pDoodad->SetWayPt(g_world->WaypointGet(wpStr));
						pWayPt = pDoodad->GetWayPoint();
					}
				}
			}

			if(pWayPt)
			{
				pWayPt->SetMode(WaypointGetMode(wpModeStr));
				pWayPt->Reverse(bBackward ? true : false);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntityIsWPDone(string name, int iOut)
//check to see if wp of entity is done, iOut = 1 if done, 0 otherwise
PROTECTED long S_EntityIsWPDone(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			int *pIsDone = (int *)ScriptParamGetVar(param, 1);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));
			WayptNode *pWayPt=0;

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity)
			{
				if(pEntity->GetEntityType() == ENTITY_TYPE_TATA
					|| pEntity->GetEntityType() == ENTITY_TYPE_ENEMY)
				{
					Creature *pCre = (Creature*)pEntity;
					pWayPt = pCre->GetWayPoint();
				}
				else if(pEntity->GetEntityType() == ENTITY_TYPE_OBJECT)
				{
					if(pEntity->GetSubType() == OBJECT_PLATFORM)
					{
						Platform *pPlatform = (Platform *)pEntity;
						pWayPt = pPlatform->GetWayPoint();
					}
					else if(pEntity->GetEntityType() == OBJECT_DOODAD)
					{
						Doodad *pDoodad = (Doodad*)pEntity;
						pWayPt = pDoodad->GetWayPoint();
					}
				}
			}

			if(pWayPt)
			{
				D3DXVECTOR3 wpPt;
				pWayPt->GetCurrentIndLoc(&wpPt);

				if(!pWayPt->IsDone()
					&& wpPt != pEntity->GetLoc())
				{
					*pIsDone = 0;
				}
				else
					*pIsDone = 1;
			}
			else
				*pIsDone = 1;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntitySetOBJState (string name, int state)
//set the entity object state
PROTECTED long S_EntitySetOBJState(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			int state = *(int *)ScriptParamGetVar(param, 1);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity)
			{
				if(pEntity->GetOBJ())
					OBJSetState(pEntity->GetOBJ(), state);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntityIsOBJStateEnd (string name, int iOut)
//check to see if object state has ended
PROTECTED long S_EntityIsOBJStateEnd(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			int *pIOut = (int *)ScriptParamGetVar(param, 1);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity)
			{
				if(pEntity->GetOBJ())
				{
					*pIOut = OBJIsStateEnd(pEntity->GetOBJ()) ? 1 : 0;
				}
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntitySetDir (string name, string target)
//set entity direction via target
PROTECTED long S_EntitySetDir(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *IDStr, *tgtName;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			if(ScriptParamGetType(param, 1) == eVarPtr)
				tgtName = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				tgtName = (char *)ScriptParamGetVar(param, 1);
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity && g_world)
			{
				D3DXVECTOR3 tgtV;

				if(g_world->TargetGet(tgtName, tgtV))
				{
					D3DXVECTOR3 v(tgtV - pEntity->GetLoc());

					D3DXVec3Normalize(&v,&v);

					pEntity->SetDir(v);
				}
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//EntitySetBehavior(string name, int state)
//set entity behavior (only used by certain entities)
PROTECTED long S_EntitySetBehavior(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int state;
			char *IDStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			state = *((int*)ScriptParamGetVar(param, 1));
			
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(!pEntity)
				pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntityDoodad(IDStr));

			if(pEntity && g_world)
			{
				pEntity->Callback(ENTITYMSG_CHANGEBEHAVIOR, state, 0);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}