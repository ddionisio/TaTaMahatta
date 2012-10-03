#include "tata_main.h"

#include "tata_world.h"

#include "tata_script.h"

#include "tata_globals.h"

//TargetGet (string targetname, vector3 v)
//v = target.loc
PROTECTED long S_TargetGet(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			g_world->TargetGet(str, *vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TargetSet (string targetname, vector3 v)
//target.loc = v
PROTECTED long S_TargetSet(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			g_world->TargetSet(str, *vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//TargetAdd (string targetname, vector3 v)
//add a target with given name to list
PROTECTED long S_TargetAdd(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			g_world->TargetAdd(str, *vec);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}