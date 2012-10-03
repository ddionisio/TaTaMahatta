#include "tata_main.h"

#include "tata_world.h"

#include "tata_creature_tata.h"

#include "tata_script.h"

#include "tata_globals.h"

//TataOnPlayList (string tataTypeName, int iOut)
//check to see if a particular Ta-Ta is on the play list, iOut = 1 if it is,
//0 otherwise.
PROTECTED long S_TaTaOnPlayList(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			if(g_world->TataOnPlayList(str))
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

//TataCurrentTeleport (string targetName)
//teleport current Ta-Ta to a target destination.
PROTECTED long S_TataCurrentTeleport(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			Creature *pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
				pCre->Teleport(str);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TataCurrentSet (string tataName)
//set current Ta-Ta to given Ta-Ta type name.
//the given Ta-Ta name must exist on the map.
PROTECTED long S_TataCurrentSet(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 loc;

			Id newCurTataID = CollisionGetEntity(str);

			g_world->TataChangeCurrent(newCurTataID);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TataCurrentGetTypeName (string sOut)
//fill in the given string with the current Ta-Ta's type name.
PROTECTED long S_TataCurrentGetTypeName(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			string *sOut = (string *)ScriptParamGetVar(param, 0);

			Creature *pCre = (Creature *)IDPageQuery(g_world->TataGetCurrent());

			if(pCre)
				(*sOut) = g_tataTypeName[pCre->GetSubType()];
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TataAddToPlayList (string tataName)
//add Ta-Ta to playlist if available in map
PROTECTED long S_TataAddToPlayList(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 loc;

			Id tataID = CollisionGetEntity(str);

			Creature *pCre = (Creature*)IDPageQuery(tataID);

			if(pCre)
				g_world->TataAddToPlayList(pCre);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TataDetachAndSpawnBaby (string tataName)
//detaches and respawns the baby tata this ta is carrying
PROTECTED long S_TataDetachAndSpawnBaby(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 loc;

			Id tataID = CollisionGetEntity(str);

			Creature *pCre = (Creature*)IDPageQuery(tataID);

			if(g_world && pCre && pCre->GetEntityType() == ENTITY_TYPE_TATA)
			{
				D3DXVECTOR3 spawnLoc; 

				TaTaCommon *pTata = (TaTaCommon *)(pCre);

				hOBJ cObj = pTata->DetachTaTa(&spawnLoc);

				if(cObj)
				{
					//spawn Baby TaTa
					
					D3DXVECTOR3 loc; OBJGetWorldLoc(cObj, (float*)loc);
					OBJRemoveFromParent(cObj);

					Creature *pBabe = g_world->TataSpawn(g_tataTypeName[TATA_BABYTATA], loc);

					pBabe->SetOBJ(cObj);

					if(pBabe)
					{
						pBabe->SetSpawnLoc(spawnLoc);
						pBabe->SetLoc(spawnLoc);
					}
				}
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}