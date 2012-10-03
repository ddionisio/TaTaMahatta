#include "script.h"
#include "script_i.h"

#include "script_variable.h"
#include "script_param.h"

#include "script_condition.h"

typedef list<CondDat> CondDatStack;		//treat it as a stack

char *g_condName[eConditionNum] = {0, "If", "ElseIF", "Else", "While"};
char *g_condTestName[eNumCondTest] = {0, "<", ">", "<=", ">=", "!=", "==", "||", "&&"};

/////////////////////////////////////
// Name:	ScriptCondGetType
// Purpose:	get condition type based
//			on given string
// Output:	none
// Return:	condition type (eCondNone if no match)
/////////////////////////////////////
PROTECTED eCondition ScriptCondGetType(const char *s)
{
	for(int i = 1; i < eConditionNum; i++)
	{
		if(stricmp(s, g_condName[i]) == 0)
			return (eCondition)i;
	}

	return eCondNone;
}

/////////////////////////////////////
// Name:	ScriptCondGetTestType
// Purpose:	get condition test type based
//			on given string
// Output:	none
// Return:	condition test type (eCONDTSTNONE if no match)
/////////////////////////////////////
PROTECTED eCondTest ScriptCondGetTestType(const char *s)
{
	for(int i = 1; i < eNumCondTest; i++)
	{
		if(stricmp(s, g_condTestName[i]) == 0)
			return (eCondTest)i;
	}

	return eCONDTSTNONE;
}

/////////////////////////////////////
// Name:	ScriptCondDestroy
// Purpose:	destroys given script line
//			condition
// Output:	script line condition destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void ScriptCondDestroy(ScriptLineCond **sCond)
{
	if(*sCond)
	{
		if((*sCond)->left)
			ScriptCondDestroy(&(*sCond)->left);
		if((*sCond)->right)
			ScriptCondDestroy(&(*sCond)->right);

		SCRIPT_FREE(*sCond);

		sCond = 0;
	}
}

//if there are two variables on top of the stack
PRIVATE bool _CondStackCheckTwoVar(CondDatStack & condStack)
{
	if(condStack.size() >= 2)
	{
		CondDat itm1 = *condStack.begin();
		CondDat itm2 = *(++condStack.begin());

		//we determine if it is a variable by checking if var == eVarNULL
		if(itm1.var.paramVarType != eVarNULL)
		{
			if(itm2.var.paramVarType != eVarNULL)
				return true;
		}
	}

	//one of them is a condition test
	return false;
}

//create the stack from buff until buff is NULL
PRIVATE RETCODE _ScriptCondGetStack(CondDatStack & condStack,
									 char **buff,
									 VarPtrArray  * variables)
{
	CondDatStack tempStack;	//holds all condition test

	CondDat cond;

	char stuff[MAXCHARBUFF];

	bool bDone = false;

	while(!bDone)
	{
		bDone = ScriptParamGetItmStr(buff, stuff, MAXCHARBUFF, ' ');

		//determine if it is a condition test
		if((cond.condTest = ScriptCondGetTestType(stuff)) != eCONDTSTNONE)
		{
			cond.var.paramVarType = eVarNULL;

			//push to temporary stack
			tempStack.push_front(cond);
		}
		//it's a variable
		else
		{
			char *stuffPtr = stuff;
			ScriptParamSingleParseBuff(&stuffPtr, 0, variables, 0, 0, &cond.var);

			//push to actual stack
			condStack.push_front(cond);

			//determine if we want to pop from temp. stack
			//and push to actual stack
			if((bDone || _CondStackCheckTwoVar(condStack)) && tempStack.size() > 0)
			{
				cond = *tempStack.begin();
				tempStack.pop_front();

				//push cond to actual stack
				condStack.push_front(cond);
			}
		}
	}

	//put the rest of the temp stack tp the condStack
	while(tempStack.size() > 0)
	{
		cond = *tempStack.begin();
		tempStack.pop_front();

		//push cond to actual stack
		condStack.push_front(cond);
	}

	return RETCODE_SUCCESS;
}

PRIVATE ScriptLineCond * _CondBuildTree(CondDatStack & condStack)
{
	//there should at least be one item
	if(condStack.size() == 0) return 0;

	ScriptLineCond *newCond = (ScriptLineCond *)SCRIPT_MALLOC(sizeof(ScriptLineCond));

	if(!newCond) { ASSERT_MSG(0, "Unable to allocate new script line condition!", "_CondBuildTree"); return 0; }

	//pop first item
	newCond->dat = *condStack.begin();
	condStack.pop_front();

	//check if var.paramVarType != eVarNULL
	if(newCond->dat.var.paramVarType != eVarNULL)
	{
		newCond->dat.condTest = eCONDTSTNONE;
		return newCond;
	}

	//build up the branching conditions
	newCond->right = _CondBuildTree(condStack);
	newCond->left  = _CondBuildTree(condStack);

	return newCond;
}

/////////////////////////////////////
// Name:	ScriptCondCreate
// Purpose:	creates the script line
//			condition binary tree test
// Output:	stuff
// Return:	script line condition
/////////////////////////////////////
PROTECTED ScriptLineCond * ScriptCondCreate(char *buff,
											VarPtrArray	*variables)
{
	ScriptLineCond *newScriptCond = 0;

	//build the stack of condition operations and variables
	char *buffPtr = buff;

	CondDatStack condStack;

	if(_ScriptCondGetStack(condStack, &buffPtr, variables) != RETCODE_SUCCESS)
		return 0;

	//build the binary tree
	newScriptCond = _CondBuildTree(condStack);

	//we are all set...

	return newScriptCond;
}

PRIVATE bool _EvalParam(const Param & param)
{
	switch(param.paramVarType)
	{
	case eVarInt:
		if(param.dat._int != 0)
			return true;
	case eVarFloat:
		if(param.dat._float != 0)
			return true;
	case eVarStr:
		if(param._str.c_str()[0] != 0)
			return true;
	}

	return false;
}

PRIVATE RETCODE _CompareParam(const Param & param1, const Param & param2)
{
	RETCODE compRet=COMP_LT;

	switch(param1.paramVarType)
	{
	case eVarInt:
		switch(param2.paramVarType)
		{
		case eVarInt:
			if(param1.dat._int == param2.dat._int)
				compRet = COMP_EQ;
			else if(param1.dat._int > param2.dat._int)
				compRet = COMP_GT;
			break;
		case eVarFloat:
			{
				int dat2 = (int)param2.dat._float;

				if(param1.dat._int == dat2)
					compRet = COMP_EQ;
				else if(param1.dat._int > dat2)
					compRet = COMP_GT;
			}
			break;
		//this should not happen, but oh well...
		case eVarStr:
			{
				int dat2 = param2._str.size(); //strlen(param2.dat._str);

				if(param1.dat._int == dat2)
					compRet = COMP_EQ;
				else if(param1.dat._int > dat2)
					compRet = COMP_GT;
			}
			break;
		}
		break;

	case eVarFloat:
		switch(param2.paramVarType)
		{
		case eVarInt:
			{
				float dat2 = (float)param2.dat._int;

				if(param1.dat._float == dat2)
					compRet = COMP_EQ;
				else if(param1.dat._float > dat2)
					compRet = COMP_GT;
			}
			break;
		case eVarFloat:
			if(param1.dat._float == param2.dat._float)
				compRet = COMP_EQ;
			else if(param1.dat._float > param2.dat._float)
				compRet = COMP_GT;
			break;
		//this should not happen, but oh well...
		case eVarStr:
			{
				float dat2 = (float)param2._str.size();//strlen(param2.dat._str);

				if(param1.dat._float == dat2)
					compRet = COMP_EQ;
				else if(param1.dat._float > dat2)
					compRet = COMP_GT;
			}
			break;
		}
		break;

	case eVarStr:
		switch(param2.paramVarType)
		{
		//this should not happen, but oh well...
		case eVarInt:
			{
				int dat1 = param1._str.size();//strlen(param1.dat._str);

				if(dat1 == param2.dat._int)
					compRet = COMP_EQ;
				else if(dat1 > param2.dat._int)
					compRet = COMP_GT;
			}
			break;
		//this should not happen, but oh well...
		case eVarFloat:
			{
				float dat1 = (float)param1._str.size();//strlen(param1.dat._str);

				if(dat1 == param2.dat._float)
					compRet = COMP_EQ;
				else if(dat1 > param2.dat._float)
					compRet = COMP_GT;
			}
			break;
		case eVarStr:
			compRet = param1._str.compare(param2._str); //strcmp(param1.dat._str, param2.dat._str);
			break;
		}
		break;
	}

	return compRet;
}

/////////////////////////////////////
// Name:	ScriptLineCondEval
// Purpose:	evaluates the condition
// Output:	pOut filled by evaluation
// Return:	true if condition is true
/////////////////////////////////////
PROTECTED bool ScriptLineCondEval(void *owner,
								  ScriptLineCond *sCond, 
								  VarPtrArray	*variables,
								  Param *pOut)
{
	bool bLeft=false, bRight=false;
	Param paramLeft, paramRight;

	//if there is only a variable, then evaluate that variable and return
	if(sCond->dat.condTest == eCONDTSTNONE 
		&& sCond->dat.var.paramVarType != eVarNULL)
	{
		bool bEval = false;

		switch(sCond->dat.var.paramVarType)
		{
		case eVarPtr:
			{
				hVAR var = sCond->dat.var.bGlobal ? 
					ScriptTeaseGetGlobalVar(sCond->dat.var.dat._varInd) : 
					ScriptGetVar(sCond->dat.var.dat._varInd, variables);

				if(var)
				{
					ScriptVarCall(owner, var, VAR_GETBASEVAR, (WPARAM)(pOut), 0);
				}
				else
					return false;
			}
			break;
		case eVarInt:
		case eVarFloat:
		case eVarStr:
			*pOut = sCond->dat.var;
			break;
		}

		return _EvalParam(*pOut);
	}

	if(sCond->left)
		bLeft = ScriptLineCondEval(owner, sCond->left, variables, &paramLeft);

	if(sCond->right)
		bRight = ScriptLineCondEval(owner, sCond->right, variables, &paramRight);

	*pOut = paramLeft;

	RETCODE condRet = _CompareParam(paramLeft, paramRight);

	switch(sCond->dat.condTest)
	{
	case eLT:	// <
		return (condRet == COMP_LT);
	case eGT:	// >
		return (condRet == COMP_GT);
	case eLE:	// <=
		return ((condRet == COMP_LT) || (condRet == COMP_EQ));
	case eGE:	// >=
		return ((condRet == COMP_GT) || (condRet == COMP_EQ));
	case eNE:	// !=
		return (condRet != COMP_EQ);
	case eEE:	// ==
		return (condRet == COMP_EQ);
	case eOR:	// ||
		return (bLeft || bRight);
	case eAND:	// &&
		return (bLeft && bRight);
	}

	return false;
}