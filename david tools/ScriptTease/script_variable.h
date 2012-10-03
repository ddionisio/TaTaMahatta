#ifndef _script_variable_h
#define _script_variable_h

//THIS HEADER IS ONLY USED BY SCRIPT TEASE!

//internal globals
extern char *g_baseVarName[eVarNum];

/////////////////////////////////////
// Name:	ScriptVarCall
// Purpose:	call the callback of 'var'
// Output:	stuff
// Return:	callback return
/////////////////////////////////////
PROTECTED long ScriptVarCall(void *owner, hVAR var, int msg, WPARAM wparam, LPARAM lparam);

/////////////////////////////////////
// Name:	ScriptVarGetBaseType
// Purpose:	gets the base variable type,
//			e.g. int, float, string
// Output:	none
// Return:	the base var type (eVarNULL if no match)
/////////////////////////////////////
PROTECTED eBaseVar ScriptVarGetBaseType(const char *name);

/////////////////////////////////////
// Name:	ScriptVarGetType
// Purpose:	grabs a variable type from
//			index depending on given
//			name
// Output:	sIndOut filled
// Return:	true if found
/////////////////////////////////////
PROTECTED bool ScriptVarGetType(const char *name, ScriptInd *sIndOut);

/////////////////////////////////////
// Name:	ScriptVarDestroy
// Purpose:	destroys a variable
// Output:	variable destroyed
// Return:	
/////////////////////////////////////
PROTECTED void ScriptVarDestroy(void *owner, hVAR *var);

/////////////////////////////////////
// Name:	ScriptVarCreate
// Purpose:	create a variable from file
// Output:	variable added to global list
//			if bGlobal == true
// Return:	new variable
/////////////////////////////////////
PROTECTED hVAR ScriptVarCreate(void *owner, FILE *fp, const ScriptInd & sInd, bool bGlobal);

/////////////////////////////////////
// Name:	ScriptVarCompareName
// Purpose:	compares name to variable
//			(case sensitive)
// Output:	none
// Return:	true if match
/////////////////////////////////////
PROTECTED bool ScriptVarCompareName(hVAR var, const char *name);

/////////////////////////////////////
// Name:	ScriptVarGetIndex
// Purpose:	grab index from given variable
//			array that matches the name
//			(case sensitive)
// Output:	none
// Return:	the index (-1 not found)
/////////////////////////////////////
PROTECTED int ScriptVarGetIndex(const char *name, VarPtrArray *variables);

/////////////////////////////////////
// Name:	ScriptGetVar
// Purpose:	grab variable from variables
// Output:	none
// Return:	the variable
/////////////////////////////////////
PROTECTED hVAR ScriptGetVar(int ind, VarPtrArray *variables);

/////////////////////////////////////
// Name:	ScriptVarIsTypeMatch
// Purpose:	determines if var matches
//			given index var. type
// Output:	none
// Return:	true if matches
/////////////////////////////////////
PROTECTED bool ScriptVarIsTypeMatch(hVAR var, eBaseVar baseVar, const ScriptInd & indMatch);

/////////////////////////////////////
// Name:	ScriptVarGetTypeInd
// Purpose:	gets the variable type index
// Output:	none
// Return:	the script index
/////////////////////////////////////
PROTECTED ScriptInd ScriptVarGetTypeInd(hVAR var);

#endif