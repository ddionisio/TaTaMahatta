#include "script.h"
#include "script_i.h"

#include "script_basics.h"

//Useful for debug purpose, file is in text mode

//file someFile (string)
PROTECTED long FileCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	tFile *fileDat;

	switch(msg)
	{
	case VAR_CREATE:
		{
			fileDat = (tFile *)SCRIPT_MALLOC(sizeof(tFile)); assert(fileDat);

			ScriptVarSetData(var, fileDat);
		}
		break;
	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			fileDat = (tFile *)ScriptVarGetData(var);

			char *filename;

			if(ScriptParamGetType(param, 0) == eVarPtr)
				filename = (char *)((string *)ScriptParamGetVar(param, 0))->c_str();
			else
				filename = (char *)ScriptParamGetVar(param, 0);

			if(fileDat->bOpen)
				fclose(fileDat->fp);

			fileDat->fp = fopen(filename, "wt");

			fileDat->bOpen = true;
		}
		break;
	case VAR_GETBASEVAR:
		{
			//this variable shouldn't be use for condition checking!
			Param *pOut = (Param *)wparam;

			pOut->dat._int     = 0;
			pOut->paramVarType = eVarInt;
		}
		break;
	case VAR_DESTROY:
		fileDat = (tFile *)ScriptVarGetData(var);

		if(fileDat->bOpen)
			fclose(fileDat->fp);

		SCRIPT_FREE(fileDat);

		break;
	}

	return RETCODE_SUCCESS;
}