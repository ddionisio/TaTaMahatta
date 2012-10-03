#ifndef _script_condition_h
#define _script_condition_h

//THIS HEADER IS ONLY USED BY SCRIPT TEASE!

//stuff about if, elseif, else, while...etc.

//internal globals
extern char *g_condName[eConditionNum];
extern char *g_condTestName[eNumCondTest];

/////////////////////////////////////
// Name:	ScriptCondGetType
// Purpose:	get condition type based
//			on given string
// Output:	none
// Return:	condition type (eCondNone if no match)
/////////////////////////////////////
PROTECTED eCondition ScriptCondGetType(const char *s);

/////////////////////////////////////
// Name:	ScriptCondGetTestType
// Purpose:	get condition test type based
//			on given string
// Output:	none
// Return:	condition test type (eCONDTSTNONE if no match)
/////////////////////////////////////
PROTECTED eCondTest ScriptCondGetTestType(const char *s);

/////////////////////////////////////
// Name:	ScriptCondDestroy
// Purpose:	destroys given script line
//			condition
// Output:	script line condition destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void ScriptCondDestroy(ScriptLineCond **sCond);

/////////////////////////////////////
// Name:	ScriptCondCreate
// Purpose:	creates the script line
//			condition binary tree test
// Output:	stuff
// Return:	script line condition
/////////////////////////////////////
PROTECTED ScriptLineCond * ScriptCondCreate(char *buff,
											VarPtrArray	*variables);

/////////////////////////////////////
// Name:	ScriptLineCondEval
// Purpose:	evaluates the condition
// Output:	pOut filled by evaluation
// Return:	true if condition is true
/////////////////////////////////////
PROTECTED bool ScriptLineCondEval(void *owner,
								  ScriptLineCond *sCond, 
								  VarPtrArray	*variables,
								  Param *pOut);

#endif