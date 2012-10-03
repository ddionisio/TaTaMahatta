#include "script.h"
#include "script_i.h"

#include "script_basics.h"

//All functions that has got to do with strings

//concatinate two strings strcat (string out, string str)
PROTECTED long strcatCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			//assume that the first param is a variable
			if(ScriptParamGetType(param, 0) == eVarPtr)
			{
				string *sOut = (string *)ScriptParamGetVar(param, 0);
				char *str;
				
				if(ScriptParamGetType(param, 1) == eVarPtr)
					str = (char*)((string *)ScriptParamGetVar(param, 1))->c_str();
				else
					str = (char *)ScriptParamGetVar(param, 1);

				sOut->append(str);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}