#ifndef _script_h
#define _script_h

#include "..\common.h"

//defines
#define MAXCHARNAME		50		//maximum character for variable/function name
#define MAXVARTYPEASCII	255

//enumerates
typedef enum {
	eVarNULL,		//used in some places
	eVarPtr,
	eVarInt,
	eVarFloat,
	eVarStr,
	eVarNum
} eBaseVar;

typedef enum {
	eCondNone,
	eIf,
	eElseIf,
	eElse,
	eWhile,
	eConditionNum
} eCondition;

typedef enum {
	eCONDTSTNONE,	// if (hey)
	eLT,	// <
	eGT,	// >
	eLE,	// <=
	eGE,	// >=
	eNE,	// !=
	eEE,	// ==
	eOR,	// ||
	eAND,	// &&
	eNumCondTest
} eCondTest;

//script returns, used in some stuff
#define FUNCRET_DONE		1	//tells script that the function is done,
								//meaning we should move on to next line

#define FUNCRET_NOTDONE		0	//tells script that the function is not yet done,
								//will call the function until 'FUNCRET_DONE'

//variable callback messages
typedef enum {
	VAR_CREATE,
	VAR_SET,		//wparam = hPARAM
	VAR_GETBASEVAR, //wparam = Param *, only fill in 'dat' and 'paramVarType'
					//					paramVarType can only be: 
					//					eVarInt, eVarFloat or eVarStr
	VAR_DESTROY
} eVarMsg;

//function callback messages
typedef enum {
	FUNC_CREATE,
	FUNC_CALL,		//wparam = hPARAM, return: FUNCRET_DONE, FUNCRET_NOTDONE
	FUNC_DESTROY
} eFuncMsg;

//handles
typedef struct _Variable *hVAR;		//this is passed in script callback
typedef struct _Function *hFUNC;	//this is passed in script callback
typedef struct _Script	 *hSCRIPT;	//script tease thing

typedef struct _PublicParam *hPARAM;//parameter handle passed in
									//setting variables or calling functions

//structs
typedef struct _ScriptInd {
	char		ascii;	//first letter search
	int			ind;	//index (-1 if not found)
} ScriptInd;

typedef union _ParamData {
	int		_int;
	float	_float;
	int		_varInd;				//basically 'ptr', index to 'VarPtrArray'
	//char	_str[MAXCHARBUFF];		//hum...
} ParamData;

typedef struct _Param {
	ParamData	dat;
	string		_str;			//use this if paramVarType == eVarStr
	bool		bGlobal;		//if it is 'eVarPtr' then is it from global?
	eBaseVar	paramVarType;	//type of variable in 'dat'
} Param;

//Calback
typedef long (* SCRIPTCALLBACK) (void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//functions

//
// Script Tease (Use these functions first before loading any script files!!!)
//

/////////////////////////////////////
// Name:	ScriptTeaseInit
// Purpose:	initialize the script
//			engine.  This will set up
//			the basic functionalities
//			of the Script Tease
// Output:	script engine initialized
// Return:	success if we succeeded
/////////////////////////////////////
PUBLIC RETCODE ScriptTeaseInit();

/////////////////////////////////////
// Name:	ScriptTeaseDestroy
// Purpose:	destroy the script engine
// Output:	Script Tease destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptTeaseDestroy();

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
									 SCRIPTCALLBACK	callback);

/////////////////////////////////////
// Name:	ScriptTeaseAddFuncType
// Purpose:	add a function type
//			params are variable type
//			names ex: "int, int, float"
// Output:	new function type added
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE ScriptTeaseAddFuncType(const char *name, char *params,
									  SCRIPTCALLBACK callback);

//
// Script
//

/////////////////////////////////////
// Name:	ScriptLoad
// Purpose:	loads a script (tons of shit happens here)
//			NOTE: some variables/functions may
//				  require that you put something in owner
//				  beware that some will expect the 'correct owner'
// Output:	script loaded
// Return:	the script
/////////////////////////////////////
PUBLIC hSCRIPT ScriptLoad(void *owner, const char *filename, const char *begin = 0, const char *end = 0);

/////////////////////////////////////
// Name:	ScriptDestroy
// Purpose:	destroys a script
//			NOTE: some may require the owner
// Output:	script ded
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptDestroy(void *owner, hSCRIPT *script);

/////////////////////////////////////
// Name:	ScriptUpdate
// Purpose:	updates script until done
// Output:	script update
// Return:	RETCODE_BREAK if done
/////////////////////////////////////
PUBLIC RETCODE ScriptUpdate(void *owner, hSCRIPT script);

/////////////////////////////////////
// Name:	ScriptReset
// Purpose:	reset the script back to the
//			first line of code
// Output:	current line back to 0
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptReset(hSCRIPT script);

//
// Variable
//

/////////////////////////////////////
// Name:	ScriptVarGetName
// Purpose:	grabs the name of given
//			variable
// Output:	sOut filled
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptVarGetName(hVAR var, char *sOut);

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
PUBLIC void * ScriptVarGetData(hVAR var);
PUBLIC void ScriptVarSetData(hVAR var, void *data);

//
// Function
//

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
PUBLIC void * ScriptFuncGetData(hFUNC func);
PUBLIC void ScriptFuncSetData(hFUNC func, void *data);

//
// Param
//

/////////////////////////////////////
// Name:	ScriptParamGetType
// Purpose:	get the type of param
//			use this for determining
//			if param is variable/constant
// Output:	none
// Return:	type
/////////////////////////////////////
PUBLIC eBaseVar ScriptParamGetType(hPARAM param, int paramInd);

/////////////////////////////////////
// Name:	ScriptParamGetVar
// Purpose:	get data from param with
//			given index
// Output:	none
// Return:	data ptr.
/////////////////////////////////////
PUBLIC void * ScriptParamGetVar(hPARAM param, int paramInd);

/////////////////////////////////////
// Name:	ScriptParamGetVarName
// Purpose:	grab the name of the variable
//			in the parameter
// Output:	sOut is filled with the name
// Return:	true if sOut is filled
/////////////////////////////////////
PUBLIC bool ScriptParamGetVarName(hPARAM param, int paramInd, char *sOut);

#endif