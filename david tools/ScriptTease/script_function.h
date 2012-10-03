#ifndef _script_function_h
#define _script_function_h

//THIS HEADER IS ONLY USED BY SCRIPT TEASE!

/////////////////////////////////////
// Name:	ScriptFuncCall
// Purpose:	call the callback of 'func'
// Output:	stuff
// Return:	callback return
/////////////////////////////////////
PROTECTED long ScriptFuncCall(void *owner, hFUNC func, int msg, WPARAM wparam, LPARAM lparam);

/////////////////////////////////////
// Name:	ScriptFuncGetType
// Purpose:	grabs a function type from
//			index depending on given
//			name
// Output:	sIndOut filled
// Return:	true if found
/////////////////////////////////////
PROTECTED bool ScriptFuncGetType(const char *name, ScriptInd *sIndOut);

/////////////////////////////////////
// Name:	ScriptFuncDestroy
// Purpose:	destroy given function
// Output:	function destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void ScriptFuncDestroy(void *owner, hFUNC *func);

/////////////////////////////////////
// Name:	ScriptFuncCreate
// Purpose:	create the function
// Output:	new function
// Return:	the new function
/////////////////////////////////////
PROTECTED hFUNC ScriptFuncCreate(void *owner, const ScriptInd & sInd);

/////////////////////////////////////
// Name:	ScriptFuncGetTypeInd
// Purpose:	gets the function type index
// Output:	none
// Return:	the script index
/////////////////////////////////////
PROTECTED ScriptInd ScriptFuncGetTypeInd(hFUNC func);

#endif