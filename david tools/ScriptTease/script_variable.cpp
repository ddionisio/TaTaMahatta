#include "script.h"
#include "script_i.h"

#include "script_variable.h"

char *g_baseVarName[eVarNum] = {0, 0, "int", "float", "string"};

/////////////////////////////////////
// Name:	ScriptVarCall
// Purpose:	call the callback of 'var'
// Output:	stuff
// Return:	callback return
/////////////////////////////////////
PROTECTED long ScriptVarCall(void *owner, hVAR var, int msg, WPARAM wparam, LPARAM lparam)
{
	return SCRIPT_VARTYPE(var->type.ascii, var->type.ind).func(owner, var, msg, wparam, lparam);
}

/////////////////////////////////////
// Name:	ScriptVarGetBaseType
// Purpose:	gets the base variable type,
//			e.g. int, float, string
// Output:	none
// Return:	the base var type (eVarNULL if no match)
/////////////////////////////////////
PROTECTED eBaseVar ScriptVarGetBaseType(const char *name)
{
	for(int i = eVarInt; i < eVarNum; i++)
	{
		if(stricmp(name, g_baseVarName[i]) == 0)
			return (eBaseVar)i;
	}

	return eVarNULL;
}

/////////////////////////////////////
// Name:	ScriptVarGetType
// Purpose:	grabs a variable type from
//			index depending on given
//			name
// Output:	sIndOut filled
// Return:	true if found
/////////////////////////////////////
PROTECTED bool ScriptVarGetType(const char *name, ScriptInd *sIndOut)
{
	sIndOut->ascii = name[0];

	for(int i = 0; i < (*g_ScriptTease->varTypes)[sIndOut->ascii].size(); i++)
	{
		if(stricmp(name, SCRIPT_VARTYPE(sIndOut->ascii, i).name) == 0)
		{
			sIndOut->ind = i;
			return true;
		}
	}

	sIndOut->ind = -1;

	return false;
}

/////////////////////////////////////
// Name:	ScriptVarDestroy
// Purpose:	destroys a variable
// Output:	variable destroyed
// Return:	
/////////////////////////////////////
PROTECTED void ScriptVarDestroy(void *owner, hVAR *var)
{
	if(*var)
	{
		ScriptVarCall(owner, *var, VAR_DESTROY, 0, 0);

		SCRIPT_FREE(*var);
		var = 0;
	}
}

/////////////////////////////////////
// Name:	ScriptVarCreate
// Purpose:	create a variable from file
// Output:	variable added to global list
//			if bGlobal == true
// Return:	new variable
/////////////////////////////////////
PROTECTED hVAR ScriptVarCreate(void *owner, FILE *fp, const ScriptInd & sInd, bool bGlobal)
{
	hVAR newVar = (hVAR)SCRIPT_MALLOC(sizeof(Variable));

	if(!newVar) { ASSERT_MSG(0, "Unable to allocate new variable!", "ScriptVarCreate"); return 0; }

	//set type
	newVar->type = sInd;

	//get name from file
	ParserReadWordFile(fp, newVar->name, MAXCHARNAME, 0);

	if(ScriptVarCall(owner, newVar, VAR_CREATE, 0, 0) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Error initializing new variable!", "ScriptVarCreate"); ScriptVarDestroy(owner, &newVar); return 0; }

	//add to global if bGlobal is true
	if(bGlobal)
	{
		ScriptTeaseAddGlobalVar(newVar);
	}

	return newVar;
}

/////////////////////////////////////
// Name:	ScriptVarCompareName
// Purpose:	compares name to variable
//			(case sensitive)
// Output:	none
// Return:	true if match
/////////////////////////////////////
PROTECTED bool ScriptVarCompareName(hVAR var, const char *name)
{
	if(strcmp(var->name, name) == 0)
		return true;

	return false;
}

/////////////////////////////////////
// Name:	ScriptVarGetIndex
// Purpose:	grab index from given variable
//			array that matches the name
//			(case sensitive)
// Output:	none
// Return:	the index (-1 not found)
/////////////////////////////////////
PROTECTED int ScriptVarGetIndex(const char *name, VarPtrArray *variables)
{
	assert(variables);

	for(int i = 0; i < variables->size(); i++)
	{
		if(ScriptVarCompareName((*variables)[i], name))
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	ScriptGetVar
// Purpose:	grab variable from variables
// Output:	none
// Return:	the variable
/////////////////////////////////////
PROTECTED hVAR ScriptGetVar(int ind, VarPtrArray *variables)
{
	assert(ind < variables->size());

	return (*variables)[ind];
}

/////////////////////////////////////
// Name:	ScriptVarIsTypeMatch
// Purpose:	determines if var matches
//			given index var. type
// Output:	none
// Return:	true if matches
/////////////////////////////////////
PROTECTED bool ScriptVarIsTypeMatch(hVAR var, eBaseVar baseVar, const ScriptInd & indMatch)
{
	//first check if it matches the baseVar
	if(baseVar != eVarPtr)
	{
		ScriptInd varInd = ScriptVarGetTypeInd(var);

		eBaseVar varBaseType = ScriptVarGetBaseType(SCRIPT_VARTYPE(varInd.ascii, varInd.ind).name);

		if(varBaseType != eVarNULL && varBaseType == baseVar)
			return true;
	}
	else if(var->type.ascii == indMatch.ascii)
	{
		if(var->type.ind == indMatch.ind)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	ScriptVarGetName
// Purpose:	grabs the name of given
//			variable
// Output:	sOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptVarGetName(hVAR var, char *sOut)
{
	strcpy(sOut, var->name);
}

/////////////////////////////////////
// Name:	ScriptVarGetTypeInd
// Purpose:	gets the variable type index
// Output:	none
// Return:	the script index
/////////////////////////////////////
PROTECTED ScriptInd ScriptVarGetTypeInd(hVAR var)
{
	return var->type;
}

/////////////////////////////////////
// Name:	ScriptVarGetData/
//			ScriptVarSetData
// Purpose:	set and get the data of
//			variable
//			NOTE: you are responsible for
//				  freeing any allocation
//				  in 'data'
// Output:	data set/get
// Return:	the data if 'Get'
/////////////////////////////////////
PUBLIC void * ScriptVarGetData(hVAR var)
{
	return var->data;
}

PUBLIC void ScriptVarSetData(hVAR var, void *data)
{
	var->data = data;
}