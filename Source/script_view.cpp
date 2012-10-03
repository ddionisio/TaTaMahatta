#include "tata_main.h"

#include "tata_world.h"

#include "tata_script.h"

#include "tata_ID.h"

#include "tata_globals.h"

//ViewSetLoc (vector3 v)
//instant view location set
PROTECTED long S_ViewSetLoc(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 0);

			g_world->ViewSetNewPt(*vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//ViewSetTarget (vector3 v)
//instant view target set
PROTECTED long S_ViewSetTarget(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 0);

			g_world->ViewSetNewTarget(*vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//ViewSetDelay (float delay)
//set view delay from getting to destination
PROTECTED long S_ViewSetDelay(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *delay = (float *)ScriptParamGetVar(param, 0);

			g_world->ViewSetDelay((double)(*delay));
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//ViewMoveLoc (vector3 v)
//move view to location
PROTECTED long S_ViewMoveLoc(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 0);

			g_world->ViewSetPt(*vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//ViewMoveTarget (vector3 v)
//move view target to dest
PROTECTED long S_ViewMoveTarget(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 0);

			g_world->ViewSetTarget(*vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//ViewFollowWaypoint (string wayptname, int mode, float delay, int isDone)
//view will follow the waypoint with given mode.
//isDone = 1 if view still following, 0 otherwise

//WayptNode *
//PUBLIC void * ScriptFuncGetData(hFUNC func);
//PUBLIC void ScriptFuncSetData(hFUNC func, void *data);

PROTECTED long S_ViewFollowWaypoint(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	WayptNode *pWNode = (WayptNode *)ScriptFuncGetData(func);

	switch(msg)
	{
	case FUNC_CREATE:
		ScriptFuncSetData(func, 0);
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			int *mode, *isDone;
			float *delay;
			char *str;

			//check to see if we already set the waypoint
			if(!pWNode)
			{
				if(ScriptParamGetType(param, 0) == eVarPtr)
					str = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
				else
					str = (char *)ScriptParamGetVar(param, 0);

				mode   = (int *)ScriptParamGetVar(param, 1);

				delay  = (float *)ScriptParamGetVar(param, 2);

				//first, let's see if we can get the waypoint
				Id wayptID = g_world->WaypointGet(str);

				//if exists, then let's create the waypoint node
				if(GAME_IDVALID(wayptID))
				{
					pWNode = new WayptNode(wayptID);
					pWNode->SetMode((eWayPtMode)(*mode));
					pWNode->SetDelaySpd((double)(*delay));

					//set camera to move to first location
					D3DXVECTOR3 ptOut, targetOut;
					pWNode->GetCurrentLinearLoc(&ptOut, &targetOut);
					g_world->ViewSetPt(ptOut);
					g_world->ViewSetTarget(targetOut);

					ScriptFuncSetData(func, pWNode);
				}
			}
			else
			{
				isDone = (int *)ScriptParamGetVar(param, 3);

				if(g_world->ViewIsDone())
				{
					bool ret = pWNode->Update();

					*isDone = ret ? 1 : 0;

					//update the view to waypoint
					D3DXVECTOR3 ptOut, targetOut;
					pWNode->GetCurrentCurvedLoc(&ptOut, &targetOut);

					g_world->ViewSetNewPt(ptOut);
					g_world->ViewSetNewTarget(targetOut);
				}
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		delete pWNode;
		break;
	}

	return RETCODE_SUCCESS;
}