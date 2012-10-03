#include "script.h"
#include "script_i.h"

#include "script_basics.h"

//file functions

//prints integer file_print_int (file, int)
PROTECTED long file_print_intCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	tFile *fileDat;

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			fileDat  = (tFile *)ScriptParamGetVar(param, 0);
			int iDat = *(int *)ScriptParamGetVar(param, 1);

			char varName[MAXCHARBUFF];

			if(fileDat->bOpen)
			{
				if(ScriptParamGetVarName(param, 1, varName))
					fprintf(fileDat->fp, "int %s=%d\n", varName, iDat);
				else
					fprintf(fileDat->fp, "%d\n", iDat);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//prints float file_print_float (file, float)
PROTECTED long file_print_floatCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	tFile *fileDat;

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			fileDat  = (tFile *)ScriptParamGetVar(param, 0);
			float fDat = *(float *)ScriptParamGetVar(param, 1);

			char varName[MAXCHARBUFF];

			if(fileDat->bOpen)
			{
				if(ScriptParamGetVarName(param, 1, varName))
					fprintf(fileDat->fp, "float %s=%f\n", varName, fDat);
				else
					fprintf(fileDat->fp, "%f\n", fDat);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//prints string file_print_string (file, string)
PROTECTED long file_print_stringCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hPARAM param;
	hFUNC func = (hFUNC)handle; assert(func);

	tFile *fileDat;

	switch(msg)
	{
	case FUNC_CREATE:
		break;
	case FUNC_CALL:
		{
			param = (hPARAM)wparam;

			fileDat  = (tFile *)ScriptParamGetVar(param, 0);

			char *strDat;

			if(ScriptParamGetType(param, 1) == eVarPtr)
				strDat = (char *)((string *)ScriptParamGetVar(param, 1))->c_str();
			else
				strDat = (char *)ScriptParamGetVar(param, 1);

			char varName[MAXCHARBUFF];

			if(fileDat->bOpen)
			{
				if(ScriptParamGetVarName(param, 1, varName))
					fprintf(fileDat->fp, "string %s=%s\n", varName, strDat);
				else
					fprintf(fileDat->fp, "%s\n", strDat);
			}
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}