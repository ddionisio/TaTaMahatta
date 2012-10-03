#include "script.h"
#include "script_i.h"

#include "script_function.h"

/////////////////////////////////////
// Name:	ScriptFuncCall
// Purpose:	call the callback of 'func'
// Output:	stuff
// Return:	callback return
/////////////////////////////////////
PROTECTED long ScriptFuncCall(void *owner, hFUNC func, int msg, WPARAM wparam, LPARAM lparam)
{
	return SCRIPT_FUNCTYPE(func->type.ascii, func->type.ind).func(owner, func, msg, wparam, lparam);
}

/////////////////////////////////////
// Name:	ScriptFuncGetType
// Purpose:	grabs a function type from
//			index depending on given
//			name
// Output:	sIndOut filled
// Return:	true if found
/////////////////////////////////////
PROTECTED bool ScriptFuncGetType(const char *name, ScriptInd *sIndOut)
{
	sIndOut->ascii = name[0];

	for(int i = 0; i < (*g_ScriptTease->funcTypes)[sIndOut->ascii].size(); i++)
	{
		if(stricmp(name, SCRIPT_FUNCTYPE(sIndOut->ascii, i).name) == 0)
		{
			sIndOut->ind = i;
			return true;
		}
	}

	sIndOut->ind = -1;

	return false;
}

/////////////////////////////////////
// Name:	ScriptFuncDestroy
// Purpose:	destroy given function
// Output:	function destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void ScriptFuncDestroy(void *owner, hFUNC *func)
{
	if(*func)
	{
		ScriptFuncCall(owner, *func, FUNC_DESTROY, 0, 0);

		SCRIPT_FREE(*func);
		func = 0;
	}
}

/////////////////////////////////////
// Name:	ScriptFuncCreate
// Purpose:	create the function
// Output:	new function
// Return:	the new function
/////////////////////////////////////
PROTECTED hFUNC ScriptFuncCreate(void *owner, const ScriptInd & sInd)
{
	hFUNC newFunc = (hFUNC)SCRIPT_MALLOC(sizeof(Function));

	if(!newFunc) { ASSERT_MSG(0, "Unable to allocate new function!", "ScriptFuncCreate"); return 0; }

	//set type
	newFunc->type = sInd;

	//initialize
	if(ScriptFuncCall(owner, newFunc, FUNC_CREATE, 0, 0) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Error initializing new variable!", "ScriptFuncCreate"); ScriptFuncDestroy(owner, &newFunc); return 0; }

	return newFunc;
}

/////////////////////////////////////
// Name:	ScriptFuncGetTypeInd
// Purpose:	gets the function type index
// Output:	none
// Return:	the script index
/////////////////////////////////////
PROTECTED ScriptInd ScriptFuncGetTypeInd(hFUNC func)
{
	return func->type;
}

/////////////////////////////////////
// Name:	ScriptFuncGetData/
//			ScriptFuncSetData
// Purpose:	set and get the data of
//			function
//			NOTE: you are responsible for
//				  freeing any allocation
//				  in 'data'
// Output:	data set/get
// Return:	the data if 'Get'
/////////////////////////////////////
PUBLIC void * ScriptFuncGetData(hFUNC func)
{
	return func->data;
}

PUBLIC void ScriptFuncSetData(hFUNC func, void *data)
{
	func->data = data;
}