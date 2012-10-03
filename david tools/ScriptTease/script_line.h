#ifndef _script_line_h
#define _script_line_h

//THIS HEADER IS ONLY USED BY SCRIPT TEASE!

//script line returns
#define		SCRIPTLINERET_NOTDONE		0
#define		SCRIPTLINERET_DONE			1
#define		SCRIPTLINERET_CONDITIONFAIL 2	// 'if' or 'else if' fails

//ScriptLine stuff

typedef enum {
	eScriptLine_Function,	//if calling a function
	eScriptLine_VarSet,		//if setting a variable
	eScriptLine_Condition	//if it is a condition
} eScriptLineInit;

/////////////////////////////////////
// Name:	ScriptLineDestroy
// Purpose:	destroys given script line
// Output:	script line destroyed
// Return:	none
/////////////////////////////////////
PROTECTED void ScriptLineDestroy(void *owner, ScriptLine **sLine);

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
										eScriptLineInit initType);

/////////////////////////////////////
// Name:	ScriptLineReset
// Purpose:	resets a line, only useful for
//			condition lines
// Output:	sub line set
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptLineReset(ScriptLine  *sLine);

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
								 RETCODE prevRetCode);

#endif