#include "script.h"
#include "script_i.h"

#include "script_parse.h"
#include "script_variable.h"
#include "script_function.h"
#include "script_param.h"
#include "script_condition.h"

#include "script_line.h"

/////////////////////////////////////
// Name:	ScriptLineDestroy
// Purpose:	destroys given script line
// Output:	script line destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void ScriptLineDestroy(void *owner, ScriptLine **sLine)
{
	if(*sLine)
	{
		//destroy sub lines
		if((*sLine)->subLines)
		{
			for(int i = 0; i < (*sLine)->subLines->size(); i++)
				ScriptLineDestroy(owner, &(*(*sLine)->subLines)[i]);

			delete (*sLine)->subLines;
		}

		//destroy function
		if((*sLine)->func)
			ScriptFuncDestroy(owner, &(*sLine)->func);

		//destroy script line condition
		if((*sLine)->condDat)
			ScriptCondDestroy(&(*sLine)->condDat);

		//destroy params
		delete (*sLine)->params;

		SCRIPT_FREE(*sLine);

		sLine = 0;
	}
}

/////////////////////////////////////
// Name:	ScriptLineCreate
// Purpose:	creates a script line
//			depending on initType:
//			func call -> sInd -> function type call
//			var set   -> sInd.ind -> variable index in variables, also uses bGlobal
//			condition -> sInd.ind -> (eCondition)
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED ScriptLine * ScriptLineCreate(void			*owner, 
										FILE			*fp, 
										VarPtrArray		*variables,
										const ScriptInd & sInd,
										bool			bGlobal,
										eScriptLineInit initType)
{
	ScriptLine *newScriptLine = (ScriptLine *)SCRIPT_MALLOC(sizeof(ScriptLine));

	if(!newScriptLine) { ASSERT_MSG(0, "Unable to allocate new script line!", "ScriptLineCreate"); return 0; }

	//do something depending on init type
	switch(initType)
	{
	case eScriptLine_Function:
		{
			//initialize function
			newScriptLine->func = ScriptFuncCreate(owner, sInd); assert(newScriptLine->func);

			/////////////////////////////////////////////////////////////
			//create parameters
			newScriptLine->params = new ParamArray; assert(newScriptLine->params);

			//get parameters
			char *buff;
			int buffSize;

			if(ParserCountStringFile(fp, &buffSize, '(', ')') != RETCODE_ENDREACHED)
			{ ASSERT_MSG(0, "Error reading parameters", "ScriptLineCreate"); ScriptLineDestroy(owner, &newScriptLine); return 0; }

			buff = (char*)SCRIPT_MALLOC(sizeof(char)*(buffSize+1));

			ParserReadStringFile(fp, buff, buffSize+1, '(', ')');

			//now parse the buffer containing stuff inside '(...)'
			//insert them in param
			ScriptInd fInd = ScriptFuncGetTypeInd(newScriptLine->func);

			ScriptParamParseBuff(buff, variables,
				&SCRIPT_FUNCTYPE(fInd.ascii, fInd.ind).funcParam,
				newScriptLine->params);

			SCRIPT_FREE(buff);
			/////////////////////////////////////////////////////////////
		}
		break;
	case eScriptLine_VarSet:
		{
			//set var ind
			newScriptLine->var.bGlobal = bGlobal;
			newScriptLine->var.dat._varInd = sInd.ind;
			newScriptLine->var.paramVarType = eVarPtr;

			//get the variable
			hVAR var = bGlobal ? ScriptTeaseGetGlobalVar(sInd.ind) : ScriptGetVar(sInd.ind, variables);

			assert(var); //this can't be null!

			/////////////////////////////////////////////////////////////
			//create parameters
			newScriptLine->params = new ParamArray; assert(newScriptLine->params);

			//get parameters
			char *buff;
			int buffSize;

			if(ParserCountStringFile(fp, &buffSize, '(', ')') != RETCODE_ENDREACHED)
			{ ASSERT_MSG(0, "Error reading parameters", "ScriptLineCreate"); ScriptLineDestroy(owner, &newScriptLine); return 0; }

			buff = (char*)SCRIPT_MALLOC(sizeof(char)*(buffSize+1));

			ParserReadStringFile(fp, buff, buffSize+1, '(', ')');

			//now parse the buffer containing stuff inside '(...)'
			//insert them in param
			ScriptInd vInd = ScriptVarGetTypeInd(var);

			ScriptParamParseBuff(buff, variables,
				&SCRIPT_VARTYPE(vInd.ascii, vInd.ind).funcParam,
				newScriptLine->params);

			SCRIPT_FREE(buff);
			/////////////////////////////////////////////////////////////
		}
		break;
	case eScriptLine_Condition:
		//extra precaution...
		newScriptLine->func = 0;
		newScriptLine->var.paramVarType = eVarNULL;

		//intialize some stuff
		newScriptLine->curSubLine = -1; //this is so that we only check the condition
										//once
		newScriptLine->prevRet    = SCRIPTLINERET_DONE;

		//create script condition
		newScriptLine->condition = (eCondition)sInd.ind;

		/////////////////////////////////////////////////////////////
		//create conditions
		if(newScriptLine->condition != eCondNone 
			&& newScriptLine->condition != eElse)
		{
			//get conditions tests, eg: if (...)
			char *buff;
			int buffSize;

			if(ParserCountStringFile(fp, &buffSize, '(', ')') != RETCODE_ENDREACHED)
			{ ASSERT_MSG(0, "Error reading parameters", "ScriptLineCreate"); ScriptLineDestroy(owner, &newScriptLine); return 0; }

			buff = (char*)SCRIPT_MALLOC(sizeof(char)*(buffSize+1));

			ParserReadStringFile(fp, buff, buffSize+1, '(', ')');

			newScriptLine->condDat = ScriptCondCreate(buff, variables);

			SCRIPT_FREE(buff);
		}
		/////////////////////////////////////////////////////////////

		//allocate the sub lines
		newScriptLine->subLines = new ScriptLinePtrArray; assert(newScriptLine->subLines);

		//now get the sub lines inside the condition
		if(ScriptParseFile(owner, fp, variables, newScriptLine->subLines, "{", "}") != RETCODE_SUCCESS)
		{ ScriptLineDestroy(owner, &newScriptLine); return 0; }

		break;
	}

	return newScriptLine;
}

/////////////////////////////////////
// Name:	ScriptLineReset
// Purpose:	resets a line, only useful for
//			condition lines
// Output:	sub line set
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptLineReset(ScriptLine  *sLine)
{
	sLine->curSubLine = -1;
}

/////////////////////////////////////
// Name:	ScriptLineExecute
// Purpose:	executes the given script line
//			the prevRetCode is used for checking
//			the next 'else if' if previous
//			'if'/'else if' fails
// Output:	stuff happens
// Return:	SCRIPTLINERET_NOTDONE		= do not go to next line yet...
//			SCRIPTLINERET_DONE			= everything is all set, proceed to next line
//			SCRIPTLINERET_CONDITIONFAIL = 'if' or 'else if' fails, proceed to next line
/////////////////////////////////////
PUBLIC RETCODE ScriptLineExecute(void *owner,
								 ScriptLine  *sLine,
								 VarPtrArray *variables,
								 RETCODE prevRetCode)
{
	//determine what to do with this line

	//is it a function call?
	if(sLine->func)
	{
		PublicParam params;

		params.variables = variables;
		params.params    = sLine->params;

		if(ScriptFuncCall(owner, sLine->func, FUNC_CALL, (WPARAM)(&params), 0) 
			== FUNCRET_NOTDONE)
			return SCRIPTLINERET_NOTDONE;
	}
	//is it setting a variable?
	else if(sLine->var.paramVarType != eVarNULL)
	{
		PublicParam params;

		params.variables = variables;
		params.params    = sLine->params;

		hVAR var = sLine->var.bGlobal ? 
			ScriptTeaseGetGlobalVar(sLine->var.dat._varInd) : 
			ScriptGetVar(sLine->var.dat._varInd, variables);

		if(var)
			ScriptVarCall(owner, var, VAR_SET, (WPARAM)(&params), 0);
	}
	//then it must be a condition line
	else
	{
		//this means that the 'if' already succeeded, no need to check for
		//'elseif' and 'else'
		if((sLine->condition == eElseIf || sLine->condition == eElse)
			&& prevRetCode == SCRIPTLINERET_DONE)
			return SCRIPTLINERET_DONE;
		//the previous condition failed
		//and that all we have is an else,
		//therefore we should execute the code within
		else if((sLine->condition == eElse)
			&& prevRetCode == SCRIPTLINERET_CONDITIONFAIL)
			sLine->curSubLine = 0;

		if(sLine->condDat
			&& sLine->curSubLine == -1
			&& sLine->condition != eElse)
		{
			Param paramStuff;

			bool bCond = ScriptLineCondEval(owner, sLine->condDat, variables, &paramStuff);

			if(!bCond)
				return SCRIPTLINERET_CONDITIONFAIL;
			else
				sLine->curSubLine = 0;
		}
		
		if(sLine->subLines && sLine->curSubLine < sLine->subLines->size())
		{
			sLine->prevRet = ScriptLineExecute(owner,
									 (*sLine->subLines)[sLine->curSubLine],
									 variables,
									 sLine->prevRet);

			if(sLine->prevRet == SCRIPTLINERET_DONE
				|| sLine->prevRet == SCRIPTLINERET_CONDITIONFAIL)
				sLine->curSubLine++;

			if(sLine->curSubLine >= sLine->subLines->size())
			{
				sLine->curSubLine = -1;

				//we are done, if condition is not 'while'
				if(sLine->condition != eWhile)
					return SCRIPTLINERET_DONE;
			}

			return SCRIPTLINERET_NOTDONE;
		}
	}

	return SCRIPTLINERET_DONE;
}