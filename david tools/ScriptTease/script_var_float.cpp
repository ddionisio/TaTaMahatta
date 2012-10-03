#include "script.h"
#include "script_i.h"

#include "script_basics.h"

struct vect {
	float x,y;
};

PUBLIC long VectorCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	vect *vDat = (vect *)ScriptVarGetData(var);

	switch(msg)
	{
	case VAR_CREATE:
		vDat = (vect*)SCRIPT_MALLOC(sizeof(vect)); assert(vDat);

		vDat->x = 0;
		vDat->y = 0;

		ScriptVarSetData(var, vDat);
		break;

	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			vDat->x = *(float*)ScriptParamGetVar(param, 0);
			vDat->y = *(float*)ScriptParamGetVar(param, 1);
		}
		break;

	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			pOut->paramVarType = eVarInt;

			if(vDat->x != 0 && vDat->y != 0)
				pOut->dat._int = 1;
			else
				pOut->dat._int = 0;
		}
		break;

	case VAR_DESTROY:
		SCRIPT_FREE(ScriptVarGetData(var));
		break;
	}

	return RETCODE_SUCCESS;
}

PROTECTED long FloatCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	float *fDat;

	switch(msg)
	{
	case VAR_CREATE:
		{
			fDat = (float*)SCRIPT_MALLOC(sizeof(float)); assert(fDat);

			ScriptVarSetData(var, fDat);
		}
		break;
	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			fDat = (float*)ScriptVarGetData(var);

			memcpy(fDat, ScriptParamGetVar(param, 0), sizeof(float));
		}
		break;
	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			pOut->dat._float   = *(float*)ScriptVarGetData(var);
			pOut->paramVarType = eVarFloat;
		}
		break;
	case VAR_DESTROY:
		SCRIPT_FREE(ScriptVarGetData(var));
		break;
	}

	return RETCODE_SUCCESS;
}