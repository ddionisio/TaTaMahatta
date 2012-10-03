#include "script.h"
#include "script_i.h"

#include "script_variable.h"

#include "script_param.h"

/////////////////////////////////////
// Name:	ScriptParamGetItmStr
// Purpose:	grab item from buff with
//			sOut, stops till it reaches endChar
// Output:	sOut
// Return:	true if buff ends
/////////////////////////////////////
PROTECTED bool ScriptParamGetItmStr(char **buff, char *sOut, 
									int sSize, const char endChar)
{
	bool bDone=false;

	//ParserSkipCharBuff(char **theBuff, const char thing, bool returnFalseOnOthers)
	char *buffTemp = *buff;

	if(ParserSkipCharBuff(&buffTemp, '"', true) == RETCODE_SUCCESS)
	{
		RETCODE parseRet;
		
		sOut[0] = '"';
		
		parseRet = ParserReadStringBuff(buff, &sOut[1], sSize-1, 
									   '"', '"');

		strcat(sOut, "\"");

		if(parseRet == RETCODE_FAILURE)
			bDone = true;
		else if(ParserSkipCharBuff(buff, endChar, true) == RETCODE_FAILURE)
			bDone = true;
		//if(ParserReadStringBuff(buff, sOut, sSize, 
		//							   0, endChar) == RETCODE_FAILURE)
		//							   bDone =true;
	}
	else if(ParserReadWordBuff(buff, sOut, sSize, endChar) == 0)
		bDone=true;	//we got to the end of 'buff'

	return bDone;
}

/////////////////////////////////////
// Name:	ScriptParamSingleParseBuff
// Purpose:	parse out one item from buff
//			with indicated endChar
//			NOTE: you don't have to pass in
//				  a funcParam
// Output:	buff is moved and paramOut
//			is set
// Return:	true if buff reached end
/////////////////////////////////////
PROTECTED bool ScriptParamSingleParseBuff(char			    **buff,
										  const char	    endChar,
										  VarPtrArray	    *variables,
										  FuncParamArray	*funcParam,
										  int				funcParamInd,
										  Param			    *paramOut)
{
	bool bDone=false;
	int varInd;
	char stuff[MAXCHARBUFF];

	bDone = ScriptParamGetItmStr(buff, stuff, MAXCHARBUFF, endChar);

	//check if it is inside the variables 'local'
	if((varInd = ScriptVarGetIndex(stuff, variables)) != -1)
	{
		paramOut->dat._varInd = varInd;
		paramOut->bGlobal = false;
		paramOut->paramVarType = eVarPtr;

		//check if it matches the function param
		if(funcParam)
		{
			if(!ScriptVarIsTypeMatch((*variables)[varInd], (*funcParam)[funcParamInd].baseVar, (*funcParam)[funcParamInd].varTypeInd))
			{ ASSERT_MSG(0, "Given variable does not match type in param", "ScriptParamSingleParseBuff"); return true; }
		}
	}
	//check if it is a global variable
	else if((varInd = ScriptTeaseGetGlobalVarInd(stuff)) != -1)
	{
		paramOut->dat._varInd = varInd;
		paramOut->bGlobal = true;
		paramOut->paramVarType = eVarPtr;

		//check if it matches the function param
		if(funcParam)
		{
			if(!ScriptVarIsTypeMatch(ScriptTeaseGetGlobalVar(varInd), (*funcParam)[funcParamInd].baseVar, (*funcParam)[funcParamInd].varTypeInd))
			{ ASSERT_MSG(0, "Given variable does not match type in param", "ScriptParamSingleParseBuff"); return true; }
		}
	}
	//check if it is a constant: int,float,string
	else
	{
		char *stuffPtr = stuff;

		paramOut->bGlobal = false;

		if(funcParam)
		{
			switch((*funcParam)[funcParamInd].baseVar)
			{
			case eVarInt:
				sscanf(stuff, "%d", &paramOut->dat._int);
				paramOut->paramVarType = eVarInt;
				break;
			case eVarFloat:
				sscanf(stuff, "%f", &paramOut->dat._float);
				paramOut->paramVarType = eVarFloat;
				break;
			case eVarStr:
				{
				/*if(ParserReadStringBuff(&stuffPtr, paramOut->dat._str, MAXCHARBUFF, 
									   '"', '"') != RETCODE_ENDREACHED)
				{ ASSERT_MSG(0, "Unable to read string from param", "ScriptParamSingleParseBuff"); return true; }
				paramOut->paramVarType = eVarStr;*/
					int strSize;
					ParserCountStringBuff(stuffPtr, &strSize, '"', '"');

					strSize++;

					char *tempStr = (char*)SCRIPT_MALLOC(sizeof(char)*strSize); assert(tempStr);

					if(ParserReadStringBuff(&stuffPtr, tempStr, strSize, 
									   '"', '"') != RETCODE_ENDREACHED)
					{ ASSERT_MSG(0, "Unable to read string from param", "ScriptParamSingleParseBuff"); SCRIPT_FREE(tempStr); return true; }

					paramOut->_str = tempStr;
					paramOut->paramVarType = eVarStr;

					SCRIPT_FREE(tempStr);
				}
				break;
			default:
				ASSERT_MSG(0, "Unknown parameter", "ScriptParamSingleParseBuff");
				return true;
			}
		}
		//...oh boy...
		else
		{
			//determine if it is a string, just check the first char
			//if it is '"'
			if(stuff[0] == '"')
			{
				char *stuffPtr = stuff;

				/*if(ParserReadStringBuff(&stuffPtr, paramOut->dat._str, MAXCHARBUFF, 
									   '"', '"') != RETCODE_ENDREACHED)
				{ ASSERT_MSG(0, "Unable to read string from param", "ScriptParamSingleParseBuff"); return true; }
				paramOut->paramVarType = eVarStr;*/
				int strSize;
				ParserCountStringBuff(stuffPtr, &strSize, '"', '"');

				strSize++;

				char *tempStr = (char*)SCRIPT_MALLOC(sizeof(char)*strSize); assert(tempStr);

				if(ParserReadStringBuff(&stuffPtr, tempStr, strSize, 
								   '"', '"') != RETCODE_ENDREACHED)
				{ ASSERT_MSG(0, "Unable to read string from param", "ScriptParamSingleParseBuff"); SCRIPT_FREE(tempStr); return true; }

				paramOut->_str = tempStr;
				paramOut->paramVarType = eVarStr;

				SCRIPT_FREE(tempStr);
			}
			else
			{
				//determine if it is a float, just look for a '.' somewhere
				char* LastPeriod = strrchr(stuff, '.');

				if(LastPeriod)
				{
					sscanf(stuff, "%f", &paramOut->dat._float);
					paramOut->paramVarType = eVarFloat;
				}
				//then it is just an integer (assume an integer)
				else
				{
					sscanf(stuff, "%d", &paramOut->dat._int);
					paramOut->paramVarType = eVarInt;
				}
			}
		}
	}

	return bDone;
}

/////////////////////////////////////
// Name:	ScriptParamParseBuff
// Purpose:	determines the parameters 'params'
//			first checks the funcParam to determine
//			what to expect then checks to see if var.
//			is available in 'variables', if not, then
//			it's a constant: int,float or string
// Output:	params filled
// Return:	success if everything is set,
//			failure if one or more params do not match 'funcParam'
//			or too few params
/////////////////////////////////////
PROTECTED RETCODE ScriptParamParseBuff(char				*buff, 
									   VarPtrArray		*variables,
									   FuncParamArray	*funcParam,
									   ParamArray		*params)
{
	char *buffWalk = buff;

	bool bDone=false;

	for(int i = 0; i < funcParam->size(); i++)
	{
		Param newParam;

		bDone = ScriptParamSingleParseBuff(&buffWalk, ',', variables, 
			funcParam, i, &newParam);

		params->push_back(newParam);

		if(bDone)
			break;
	}

	if(params->size() < funcParam->size())
	{ ASSERT_MSG(0, "Too few parameters!", "ScriptParamParseBuff"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

//
// Public Param stuff
//

/////////////////////////////////////
// Name:	ScriptParamGetType
// Purpose:	get the type of param
//			use this for determining
//			if param is variable/constant
// Output:	none
// Return:	type
/////////////////////////////////////
PUBLIC eBaseVar ScriptParamGetType(hPARAM param, int paramInd)
{
	assert(param);
	assert(param->params);
	assert(param->variables);

	return (*param->params)[paramInd].paramVarType;
}

/////////////////////////////////////
// Name:	ScriptParamGetVar
// Purpose:	get data from param with
//			given index
// Output:	none
// Return:	data ptr.
/////////////////////////////////////
PUBLIC void * ScriptParamGetVar(hPARAM param, int paramInd)
{
	assert(param);
	assert(param->params);
	assert(param->variables);

	//check if it is a constant
	if((*param->params)[paramInd].paramVarType != eVarPtr)
	{
		switch((*param->params)[paramInd].paramVarType)
		{
		case eVarInt:
			return &(*param->params)[paramInd].dat._int;
		case eVarFloat:
			return &(*param->params)[paramInd].dat._float;
		case eVarStr:
			return (void*)(*param->params)[paramInd]._str.c_str();
		}
	}
	else
	{
		int ind = (*param->params)[paramInd].dat._varInd;

		hVAR var = (*param->params)[paramInd].bGlobal ?
			ScriptTeaseGetGlobalVar(ind) :
			ScriptGetVar(ind, param->variables);

		return ScriptVarGetData(var);
	}

	return 0;
}

/////////////////////////////////////
// Name:	ScriptParamGetVarName
// Purpose:	grab the name of the variable
//			in the parameter
// Output:	sOut is filled with the name
// Return:	true if sOut is filled
/////////////////////////////////////
PUBLIC bool ScriptParamGetVarName(hPARAM param, int paramInd, char *sOut)
{
	assert(param);
	assert(param->params);
	assert(param->variables);

	//it has got to be a 'ptr'
	if((*param->params)[paramInd].paramVarType == eVarPtr)
	{
		int ind = (*param->params)[paramInd].dat._varInd;

		hVAR var = (*param->params)[paramInd].bGlobal ?
			ScriptTeaseGetGlobalVar(ind) :
			ScriptGetVar(ind, param->variables);

		if(var)
		{
			ScriptVarGetName(var, sOut);

			return true;
		}
	}

	sOut[0] = 0;
	return false;
}