#include "script.h"
#include "script_i.h"

#include "script_basics.h"

PROTECTED long StringCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	string *sDat;

	switch(msg)
	{
	case VAR_CREATE:
		{
			sDat = new string;

			ScriptVarSetData(var, sDat);
		}
		break;
	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			sDat = (string *)ScriptVarGetData(var);

			char *paramString;

			if(ScriptParamGetType(param, 0) == eVarPtr)
				paramString = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				paramString = (char *)ScriptParamGetVar(param, 0);

			if(paramString)
			{
				(*sDat) = paramString;
			}
		}
		break;
	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			sDat = (string*)ScriptVarGetData(var);

			if(sDat->compare("\0") == 0)
			{
				pOut->dat._int     = 0;
				pOut->paramVarType = eVarInt;
			}
			else
			{
				//strcpy(pOut->dat._str, sDat);
				pOut->_str = *sDat;
				pOut->paramVarType = eVarStr;
			}
		}
		break;
	case VAR_DESTROY:
		SCRIPT_FREE(ScriptVarGetData(var));
		break;
	}

	return RETCODE_SUCCESS;
}