#include "script.h"
#include "script_i.h"

#include "script_basics.h"

PROTECTED long IntCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	int *iDat;

	switch(msg)
	{
	case VAR_CREATE:
		{
			iDat = (int*)SCRIPT_MALLOC(sizeof(float)); assert(iDat);

			ScriptVarSetData(var, iDat);
		}
		break;
	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			iDat = (int*)ScriptVarGetData(var);

			memcpy(iDat, ScriptParamGetVar(param, 0), sizeof(int));
		}
		break;
	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			pOut->dat._int     = *(int*)ScriptVarGetData(var);
			pOut->paramVarType = eVarInt;
		}
		break;
	case VAR_DESTROY:
		SCRIPT_FREE(ScriptVarGetData(var));
		break;
	}

	return RETCODE_SUCCESS;
}