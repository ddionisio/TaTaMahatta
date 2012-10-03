#include "tata_main.h"

#include "tata_world.h"

#include "tata_object_common.h"

#include "tata_script.h"

#include "tata_globals.h"

//ObjectActivate (string objname, int bActive)
//set the object active/inactive
PROTECTED long S_ObjectActivate(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			//CollisionGetEntity(const char *name)
			param = (hPARAM)wparam;

			char *IDStr;
			int *pbActive;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			pbActive = (int *)ScriptParamGetVar(param, 1);

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(pEntity && pEntity->GetEntityType() == ENTITY_TYPE_OBJECT)
				pEntity->SetFlag(OBJ_FLAG_INACTIVE, *pbActive == 0 ? true : false);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//ObjectISActive (string objname, int iOut)
//is the object active? iOut = 0 (no) iOut = 1 (yes)
PROTECTED long S_ObjectISActive(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			//CollisionGetEntity(const char *name)
			param = (hPARAM)wparam;

			char *IDStr;
			int *iOut;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			iOut = (int *)ScriptParamGetVar(param, 1);

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(pEntity && pEntity->GetEntityType() == ENTITY_TYPE_OBJECT)
			{
				*iOut = pEntity->CheckFlag(OBJ_FLAG_INACTIVE) ? 0 : 1;
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//PlatformSetWPMode (string platformname, string mode)
//set the platform waypoint
PROTECTED long S_PlatformSetWPMode(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			//CollisionGetEntity(const char *name)
			param = (hPARAM)wparam;

			char *IDStr, *modeStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			if(ScriptParamGetType(param, 1) == eVarPtr)
				modeStr = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				modeStr = (char *)ScriptParamGetVar(param, 1);

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(pEntity 
				&& pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
				&& pEntity->GetSubType() == OBJECT_PLATFORM)
			{
				Platform *pPlatform = (Platform *)pEntity;

				WayptNode *pWP = pPlatform->GetWayPoint();

				if(pWP)
					pWP->SetMode(WaypointGetMode(modeStr));
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//PlatformWPBackward (string platformname, int bBackward)
//set the platform backward or forward
PROTECTED long S_PlatformWPBackward(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			//CollisionGetEntity(const char *name)
			param = (hPARAM)wparam;

			char *IDStr;
			int *pbBackward;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			pbBackward = (int *)ScriptParamGetVar(param, 1);

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(pEntity 
				&& pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
				&& pEntity->GetSubType() == OBJECT_PLATFORM)
			{
				Platform *pPlatform = (Platform *)pEntity;

				WayptNode *pWP = pPlatform->GetWayPoint();

				if(pWP)
					pWP->Reverse(*pbBackward == 1 ? true : false);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//PlatformISWPBackward (string platformname, int iOut)
//is the platform backward? iOut = 0 (no) iOut = 1 (yes)
PROTECTED long S_PlatformISWPBackward(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			//CollisionGetEntity(const char *name)
			param = (hPARAM)wparam;

			char *IDStr;
			int *iOut;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				IDStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				IDStr = (char *)ScriptParamGetVar(param, 0);

			iOut = (int *)ScriptParamGetVar(param, 1);

			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(IDStr));

			if(pEntity 
				&& pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
				&& pEntity->GetSubType() == OBJECT_PLATFORM)
			{
				Platform *pPlatform = (Platform *)pEntity;

				WayptNode *pWP = pPlatform->GetWayPoint();

				if(pWP)
					*iOut = pWP->IsReverse() ? 1 : 0;
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TriggerGetEntity (string sOut)
//get the entity that collided with Trigger
//NOTE: this is only used in trigger scripts
PROTECTED long S_TriggerGetEntity(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	Id *pID = (Id*)owner;

	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			//CollisionGetEntity(const char *name)
			param = (hPARAM)wparam;

			string *sOut = (string *)ScriptParamGetVar(param, 0);

			EntityCommon *pEntity = (EntityCommon*)IDPageQuery(*pID);

			if(pEntity 
				&& pEntity->GetEntityType() == ENTITY_TYPE_OBJECT
				&& pEntity->GetSubType() == OBJECT_TRIGGER)
			{
				Trigger *pTrigger = (Trigger *)pEntity;

				EntityCommon *pEntity = (EntityCommon *)IDPageQuery(pTrigger->GetCollideWith());

				if(pEntity)
					(*sOut) = pEntity->GetName();
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}