#include "script.h"
#include "script_i.h"

#include "script_basics.h"

//All functions that has got to do with integers

//increment integer inc_i (int)
PROTECTED long inc_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iDat = (int *)ScriptParamGetVar(param, 0);

			*iDat = (*iDat) + 1;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//decrement integer dec_i (int)
PROTECTED long dec_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iDat = (int *)ScriptParamGetVar(param, 0);

			*iDat = (*iDat) - 1;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//add two integers add_i (int out, int i1, int i2)
//out = i1 + i2
PROTECTED long add_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 0);
			int *i1   = (int *)ScriptParamGetVar(param, 1);
			int *i2   = (int *)ScriptParamGetVar(param, 2);

			*iOut = (*i1) + (*i2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//sub two integers sub_i (int out, int i1, int i2)
//out = i1 - i2
PROTECTED long sub_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 0);
			int *i1   = (int *)ScriptParamGetVar(param, 1);
			int *i2   = (int *)ScriptParamGetVar(param, 2);

			*iOut = (*i1) - (*i2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//mult two integers add_i (int out, int i1, int i2)
//out = i1 * i2
PROTECTED long mult_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 0);
			int *i1   = (int *)ScriptParamGetVar(param, 1);
			int *i2   = (int *)ScriptParamGetVar(param, 2);

			*iOut = (*i1) * (*i2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//div two integers div_i (int out, int i1, int i2)
//out = i1 / i2
PROTECTED long div_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 0);
			int *i1   = (int *)ScriptParamGetVar(param, 1);
			int *i2   = (int *)ScriptParamGetVar(param, 2);

			*iOut = (*i1) / (*i2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//mod two integers mod_i (int out, int i1, int i2)
//out = i1 % i2
PROTECTED long mod_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 0);
			int *i1   = (int *)ScriptParamGetVar(param, 1);
			int *i2   = (int *)ScriptParamGetVar(param, 2);

			*iOut = (*i1) % (*i2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//randomize between two integer random_i (int out, int min, int max)
//out = random [min, max]
PROTECTED long random_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			int *iOut = (int *)ScriptParamGetVar(param, 0);
			int *i1   = (int *)ScriptParamGetVar(param, 1);
			int *i2   = (int *)ScriptParamGetVar(param, 2);

			int tempnum = (*i2) - (*i1) + 1;

			*iOut = (*i2) - (rand()%tempnum);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}