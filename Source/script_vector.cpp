#include "tata_main.h"

#include "tata_script.h"

//
// Variables
//

///////////////////////////////////////
//Vector3 (float, float, float)
PROTECTED long S_Vector3Callback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
{
	hVAR var = (hVAR)handle; assert(var);

	D3DXVECTOR3 *vDat = (D3DXVECTOR3 *)ScriptVarGetData(var);

	switch(msg)
	{
	case VAR_CREATE:
		vDat = new D3DXVECTOR3; assert(vDat);

		vDat->x = 0;
		vDat->y = 0;
		vDat->z = 0;

		ScriptVarSetData(var, vDat);
		break;

	case VAR_SET:
		{
			hPARAM param = (hPARAM)wparam;

			vDat->x = *(float*)ScriptParamGetVar(param, 0);
			vDat->y = *(float*)ScriptParamGetVar(param, 1);
			vDat->z = *(float*)ScriptParamGetVar(param, 2);
		}
		break;

	case VAR_GETBASEVAR:
		{
			Param *pOut = (Param *)wparam;

			pOut->paramVarType = eVarFloat;

			pOut->dat._float = D3DXVec3LengthSq(vDat);
		}
		break;

	case VAR_DESTROY:
		delete vDat;
		break;
	}

	return RETCODE_SUCCESS;
}

//
// Functions
//

///////////////////////////////////////
//Vector3GetX (float f, vector3 vec)
//f = vec.x
PROTECTED long S_Vector3GetX(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut      = (float *)ScriptParamGetVar(param, 0);
			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			*fOut = vec->x;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

///////////////////////////////////////
//Vector3GetY (float f, vector3 vec)
//f = vec.y
PROTECTED long S_Vector3GetY(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut      = (float *)ScriptParamGetVar(param, 0);
			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			*fOut = vec->y;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

///////////////////////////////////////
//Vector3GetZ (float f, vector3 vec)
//f = vec.z
PROTECTED long S_Vector3GetZ(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			float *fOut      = (float *)ScriptParamGetVar(param, 0);
			D3DXVECTOR3 *vec = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			*fOut = vec->z;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}

///////////////////////////////////////
//Vector3Set (vector3 v1, vector3 v2)
//v1 = v2
PROTECTED long S_Vector3Set(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam)
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

			D3DXVECTOR3 *vec1 = (D3DXVECTOR3 *)ScriptParamGetVar(param, 0);
			D3DXVECTOR3 *vec2 = (D3DXVECTOR3 *)ScriptParamGetVar(param, 1);

			*vec1 = *vec2;
		}
		return FUNCRET_DONE;

	case FUNC_DESTROY:
		break;
	}

	return RETCODE_SUCCESS;
}