#include "script.h"
#include "script_i.h"

#include "script_basics.h"

//All functions that has got to do with floats

//increment float inc_f (float)
PROTECTED long inc_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fDat = (float *)ScriptParamGetVar(param, 0);

			*fDat = (*fDat) + 1;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//decrement float dec_f (float)
PROTECTED long dec_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fDat = (float *)ScriptParamGetVar(param, 0);

			*fDat = (*fDat) - 1;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//add two floats add_f (float out, float f1, float f2)
//out = f1 + f2
PROTECTED long add_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f1   = (float *)ScriptParamGetVar(param, 1);
			float *f2   = (float *)ScriptParamGetVar(param, 2);

			*fOut = (*f1) + (*f2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//sub two floats sub_f (float out, float f1, float f2)
//out = f1 - f2
PROTECTED long sub_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f1   = (float *)ScriptParamGetVar(param, 1);
			float *f2   = (float *)ScriptParamGetVar(param, 2);

			*fOut = (*f1) - (*f2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//mult two floats mult_f (float out, float f1, float f2)
//out = f1 * f2
PROTECTED long mult_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f1   = (float *)ScriptParamGetVar(param, 1);
			float *f2   = (float *)ScriptParamGetVar(param, 2);

			*fOut = (*f1) * (*f2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//div two floats div_f (float out, float f1, float f2)
//out = f1 / f2
PROTECTED long div_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f1   = (float *)ScriptParamGetVar(param, 1);
			float *f2   = (float *)ScriptParamGetVar(param, 2);

			*fOut = (*f1) * (*f2);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//sin (float out, float radian)
//out = sin(radian)
PROTECTED long sinCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut   = (float *)ScriptParamGetVar(param, 0);
			float *radian = (float *)ScriptParamGetVar(param, 1);

			*fOut = sinf(*radian);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//cos (float out, float radian)
//out = cos(radian)
PROTECTED long cosCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut   = (float *)ScriptParamGetVar(param, 0);
			float *radian = (float *)ScriptParamGetVar(param, 1);

			*fOut = cosf(*radian);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//tan (float out, float radian)
//out = tan(radian)
PROTECTED long tanCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut   = (float *)ScriptParamGetVar(param, 0);
			float *radian = (float *)ScriptParamGetVar(param, 1);

			*fOut = tanf(*radian);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//asin (float out, float f)
//out = asin(f)
PROTECTED long asinCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f    = (float *)ScriptParamGetVar(param, 1);

			*fOut = asinf(*f);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//acos (float out, float f)
//out = acos(f)
PROTECTED long acosCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f    = (float *)ScriptParamGetVar(param, 1);

			*fOut = acosf(*f);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//atan (float out, float f)
//out = atan(f)
PROTECTED long atanCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f    = (float *)ScriptParamGetVar(param, 1);

			*fOut = atanf(*f);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//sqrt (float out, float f)
//out = sqrt(f)
PROTECTED long sqrtCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f    = (float *)ScriptParamGetVar(param, 1);

			*fOut = sqrt(*f);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//degtorad (float out, float deg)
//out = deg * (PI / 180)
PROTECTED long degtoradCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *deg  = (float *)ScriptParamGetVar(param, 1);

			*fOut = (*deg) * (PI / 180.0f);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//radtodeg (float out, float rad)
//out = rad * (180 / PI)
PROTECTED long radtodegCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *rad  = (float *)ScriptParamGetVar(param, 1);

			*fOut = (*rad) * (180.0f / PI);
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

//randomize between two floats random_f (float out, float min, float max)
//out = random [min, max]
PROTECTED long random_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut = (float *)ScriptParamGetVar(param, 0);
			float *f1   = (float *)ScriptParamGetVar(param, 1);
			float *f2   = (float *)ScriptParamGetVar(param, 2);

			int r;
			float	x;
    
			r = rand();
			x = (float)(r & 0x7fff) /
				(float)0x7fff;

			*fOut = (x * ((*f2) - (*f1)) + (*f1));
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}