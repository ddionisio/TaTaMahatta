#include "script.h"
#include "script_i.h"

#include "script_variable.h"
#include "script_function.h"

MainScript * g_ScriptTease=0;

/////////////////////////////////////
// Name:	ScriptTeaseInit
// Purpose:	initialize the script
//			engine.  This will set up
//			the basic functionalities
//			of the Script Tease
// Output:	script engine initialized
// Return:	success if we succeeded
/////////////////////////////////////
PUBLIC RETCODE ScriptTeaseInit()
{
	g_ScriptTease = (MainScript *)SCRIPT_MALLOC(sizeof(MainScript));

	if(!g_ScriptTease) { ASSERT_MSG(0, "Unable to allocate script tease!", "ScriptTeaseInit"); return RETCODE_FAILURE; }

	g_ScriptTease->varTypes  = new VarTypeArray; g_ScriptTease->varTypes->resize(MAXVARTYPEASCII);
	g_ScriptTease->funcTypes = new FuncTypeArray; g_ScriptTease->funcTypes->resize(MAXVARTYPEASCII);
	g_ScriptTease->globalVar = new VarPtrArray;

	//initialize variable/function types provided
	ScriptBasicInit();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	ScriptTeaseDestroy
// Purpose:	destroy the script engine
// Output:	Script Tease destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptTeaseDestroy()
{
	if(g_ScriptTease)
	{
		if(g_ScriptTease->globalVar)
		{
			for(int i = 0; i < g_ScriptTease->globalVar->size(); i++)
				ScriptVarDestroy(0, &(*g_ScriptTease->globalVar)[i]);

			delete g_ScriptTease->globalVar;
		}

		if(g_ScriptTease->varTypes)
			delete g_ScriptTease->varTypes;

		if(g_ScriptTease->funcTypes)
			delete g_ScriptTease->funcTypes;

		SCRIPT_FREE(g_ScriptTease);

		g_ScriptTease = 0;
	}
}

PRIVATE RETCODE _ScriptTeaseGetParam(FuncParamArray & param, char *paramBuff)
{
	char buff[MAXCHARBUFF];

	char *paramBuffPtr = paramBuff;

	FuncParam fParam;

	bool bDone = false;

	while(!bDone)
	{
		if(ParserReadWordBuff(&paramBuffPtr, buff, MAXCHARBUFF, ',') == 0)
			bDone=true;	//we got to the end of 'buff'

		//check if it is one of the base variable
		if((fParam.baseVar = ScriptVarGetBaseType(buff)) != eVarNULL)
		{
			fParam.varTypeInd.ind = -1;
		}
		//check the vartype array
		else
		{
			fParam.baseVar = eVarPtr;

			if(!ScriptVarGetType(buff, &fParam.varTypeInd))
			{ ASSERT_MSG(0, "Unknown param!", "_ScriptTeaseGetParam"); return RETCODE_FAILURE; }
		}

		param.push_back(fParam);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	ScriptTeaseAddVarType
// Purpose:	add a variable type
//			params are variable type
//			names ex: "int, int, float"
// Output:	new variable type added
// Return:	success if successful
/////////////////////////////////////
PUBLIC RETCODE ScriptTeaseAddVarType(const char	*name, int size, 
									 char *params,
									 SCRIPTCALLBACK	callback)
{
	ScriptInd sInd;

	VarType newVarType;

	//first check if this variable type exists
	if(!ScriptVarGetType(name, &sInd))	//add the variable type if not found
	{
		strcpy(newVarType.name, name);
		
		newVarType.size = size;
		newVarType.func = callback;

		//add this var. type
		sInd.ind = (*g_ScriptTease->varTypes)[sInd.ascii].size();
		(*g_ScriptTease->varTypes)[sInd.ascii].push_back(newVarType);

		//now create the params
		if(_ScriptTeaseGetParam(SCRIPT_VARTYPE(sInd.ascii, sInd.ind).funcParam, params) != RETCODE_SUCCESS)
		{ (*g_ScriptTease->varTypes)[sInd.ascii].pop_back(); return RETCODE_FAILURE; }
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	ScriptTeaseAddFuncType
// Purpose:	add a function type
//			params are variable type
//			names ex: "int, int, float"
// Output:	new function type added
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ScriptTeaseAddFuncType(const char *name, char *params,
									  SCRIPTCALLBACK callback)
{
	ScriptInd sInd;

	FuncType newFuncType;

	//first check if this variable type exists
	if(!ScriptFuncGetType(name, &sInd))	//add the variable type if not found
	{
		strcpy(newFuncType.name, name);
		
		newFuncType.func = callback;

		//now create the params
		if(_ScriptTeaseGetParam(newFuncType.funcParam, params) != RETCODE_SUCCESS)
		{ return RETCODE_FAILURE; }

		//add this func. type
		(*g_ScriptTease->funcTypes)[sInd.ascii].push_back(newFuncType);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	ScriptTeaseGetGlobalVarInd
// Purpose:	grabs the global variable
//			index based on name
// Output:	none
// Return:	index, -1 if not found
/////////////////////////////////////
PROTECTED int ScriptTeaseGetGlobalVarInd(const char *name)
{
	for(int i = 0; i < g_ScriptTease->globalVar->size(); i++)
	{
		if(ScriptVarCompareName((*g_ScriptTease->globalVar)[i], name))
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	ScriptTeaseGetGlobalVar
// Purpose:	grab global variable
// Output:	none
// Return:	the global variable
/////////////////////////////////////
PROTECTED hVAR ScriptTeaseGetGlobalVar(int ind)
{
	assert(ind < g_ScriptTease->globalVar->size());

	return (*g_ScriptTease->globalVar)[ind];
}

/////////////////////////////////////
// Name:	ScriptTeaseAddGlobalVar
// Purpose:	appends a variable to the
//			global list
// Output:	global var list updated
// Return:	true if added
/////////////////////////////////////
PROTECTED bool ScriptTeaseAddGlobalVar(hVAR var)
{
	char name[MAXCHARBUFF];
	ScriptVarGetName(var, name);

	//make sure the var is not found
	if(ScriptTeaseGetGlobalVarInd(name) == -1)
	{
		//add it
		g_ScriptTease->globalVar->push_back(var);
		
		return true;
	}

	return false;
}