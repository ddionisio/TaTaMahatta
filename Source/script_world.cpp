#include "tata_main.h"

#include "tata_world.h"

#include "tata_script.h"

#include "tata_ID.h"

#include "tata_menu.h"
#include "tata_menu_game.h"

#include "tata_globals.h"

//MenuActivate (string menuName)
//activates a menu
PROTECTED long S_MenuActivate(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int t = MenuGameGetType(str);

			if(t != -1)
			{
				MenuGameAdd((eGameMenuType)t);
				MenuEnableCurrent(true);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//MenuIsAvailable (string menuName, int iOut)
//check to see if the given menu is available
//iOut = 1 if available, 0 otherwise
PROTECTED long S_MenuIsAvailable(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			if(MenuIsAvailable(str))
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

//MapLoadCloud (string target, float angle, int bRemoveCutscene)
//Load the cloud level
//angle is in degree
//bRemoveCutscene == true, remove cutscene
PROTECTED long S_MapLoadCloud(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *targetStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				targetStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				targetStr = (char *)ScriptParamGetVar(param, 0);

			float *pAngle = (float *)ScriptParamGetVar(param, 1);

			int *pRemoveCutscene = (int *)ScriptParamGetVar(param, 2);

			g_world->MapPostLoadCloud(targetStr, D3DXToRadian(*pAngle), 
				*pRemoveCutscene ? true : false);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//MapLoad (string map, string target, float angle, int bRemoveCutscene)
//Load given map, relative to game folder
//angle is in degree
//bRemoveCutscene == true, remove cutscene
PROTECTED long S_MapLoad(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *mapStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				mapStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				mapStr = (char *)ScriptParamGetVar(param, 0);

			char *targetStr;
				
			if(ScriptParamGetType(param, 1) == eVarPtr)
				targetStr = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				targetStr = (char *)ScriptParamGetVar(param, 1);

			float *pAngle = (float *)ScriptParamGetVar(param, 2);

			int *pRemoveCutscene = (int *)ScriptParamGetVar(param, 3);

			g_world->MapPostLoad(mapStr, targetStr, D3DXToRadian(*pAngle), 
				*pRemoveCutscene ? true : false);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//SetStageLevel (int stage, int level)
//set the stage and level to play.
PROTECTED long S_SetStageLevel(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			g_stage = *((int *)ScriptParamGetVar(param, 0));

			int var = *((int *)ScriptParamGetVar(param, 1));

			if(var >= 0)
				g_level = var;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//SetWorldScene (string section)
//set the world scene from a section in world scene file
PROTECTED long S_SetWorldScene(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *sectionStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				sectionStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				sectionStr = (char *)ScriptParamGetVar(param, 0);

			if(g_world)
				g_world->CutsceneSet(sectionStr);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

struct _tWait {
	bool bSet;		//timer already set?
	win32Time timer;
};

//Wait (float delay)
//wait for given milliseconds
PROTECTED long S_Wait(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	_tWait *pDat = (_tWait *)ScriptFuncGetData(func);

	switch(msg)
	{
	case FUNC_CREATE:
		pDat = new _tWait;
		
		if(pDat)
		{
			pDat->bSet = false;
			ScriptFuncSetData(func, pDat);
		}
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			float delay = *(float*)ScriptParamGetVar(param, 0);

			if(pDat)
			{
				if(!pDat->bSet)
				{
					TimeInit(&pDat->timer, delay);
					pDat->bSet = true;
				}
				else if(TimeElapse(&pDat->timer))
				{
					pDat->bSet = false;
					return FUNCRET_DONE;
				}
			}
			else
			{
				pDat->bSet = false;
				return FUNCRET_DONE;
			}
		}
		return FUNCRET_NOTDONE;

	case FUNC_DESTROY:
		if(pDat)
			delete pDat;
		break;
	}

	return RETCODE_SUCCESS;
}

//WaitBtn (float delay)
//wait for button with given milliseconds
PROTECTED long S_WaitBtn(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	_tWait *pDat = (_tWait *)ScriptFuncGetData(func);

	switch(msg)
	{
	case FUNC_CREATE:
		pDat = new _tWait;
		
		if(pDat)
		{
			pDat->bSet = false;
			ScriptFuncSetData(func, pDat);
		}
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			float delay = *(float*)ScriptParamGetVar(param, 0);

			if(pDat)
			{
				if(!pDat->bSet)
				{
					pDat->bSet = true;
					TimeInit(&pDat->timer, delay);
				}
				else if(InputAnyBtnReleased()
					|| INPXKbIsReleased(DIK_RETURN)
					|| INPXKbIsReleased(DIK_ESCAPE))
				{
					TimeReset(&pDat->timer);
					return FUNCRET_DONE;
				}
				else if(TimeElapse(&pDat->timer))
					return FUNCRET_DONE;
			}
			else
				return FUNCRET_DONE;
		}
		return FUNCRET_NOTDONE;

	case FUNC_DESTROY:
		if(pDat)
			delete pDat;
		break;
	}

	return RETCODE_SUCCESS;
}

//GoalAdd (int amt)
//adds goal with given amount
PROTECTED long S_GoalAdd(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int amt = *(int *)ScriptParamGetVar(param, 0);

			//that's all there is to do
			if(g_world)
				g_world->HUDSetGoalMinFixed(g_world->HUDGetGoalMinFixed()+amt);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//LoadCreditsScene (int bSave)
//loads up the credits (after credits, the player will be brought back to the main menu)
PROTECTED long S_LoadCreditsScene(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int bSave = *(int *)ScriptParamGetVar(param, 0);

			//that's all there is to do
			if(g_world)
				g_world->SetFlag(WORLD_FLAG_CREDITS, true);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//WorldSetMusic (string filename)
//sets the current music
PROTECTED long S_WorldSetMusic(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			char *fileStr;
				
			if(ScriptParamGetType(param, 0) == eVarPtr)
				fileStr = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				fileStr = (char *)ScriptParamGetVar(param, 0);

			if(g_world)
			{
				char buff[MAXCHARBUFF]="Main\\";
				strcat(buff, fileStr);
				/*string musPath = GAMEFOLDER;
				musPath += "\\";
				musPath += fileStr;*/

				g_world->MusicSet(buff);//musPath.c_str());
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}