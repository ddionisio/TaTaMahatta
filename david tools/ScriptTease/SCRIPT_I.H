#ifndef _script_i_h
#define _script_i_h

#pragma warning ( disable : 4786 )

#include "script.h"

#include "..\Parser\parser.h"

#define SCRIPT_MALLOC(s) calloc(1, (s))//malloc((s))
#define SCRIPT_FREE(data) free((data))

#define SCRIPT_VARTYPE(ascii, ind) (*g_ScriptTease->varTypes)[(ascii)][(ind)]
#define SCRIPT_FUNCTYPE(ascii, ind) (*g_ScriptTease->funcTypes)[(ascii)][(ind)]

typedef vector<Param>					ParamArray;
typedef vector< struct _ScriptLine * >	ScriptLinePtrArray;
typedef vector<hVAR>					VarPtrArray;

//Main data structures
typedef struct _FuncParam {
	ScriptInd	varTypeInd;		//variable type for ptr.
	eBaseVar	baseVar;		//determines what to get from param (if 'eVarPtr', use varTypeInd to compare)
} FuncParam;

typedef vector<FuncParam>				FuncParamArray;

typedef struct _VarType {
	char			name[MAXCHARNAME];//variable name type
	SCRIPTCALLBACK	func;		//callback for variable type: initialize, assign, destruction, compare...
	int				size;		//data size of this variable type

	FuncParamArray	funcParam;	//variable types expected for setting this variable
} VarType;

typedef vector< vector<VarType> >		VarTypeArray;

typedef struct _Variable {
	ScriptInd type;				//variable type (index to look-up variable types)
	char	name[MAXCHARBUFF];	//name of variable
	void	*data;				//data of variable 'this' pointer
	FLAGS	status;				//initialized or not...etc.
} Variable;

typedef struct _FuncType {
	char			name[MAXCHARNAME];//parameter name type
	SCRIPTCALLBACK	func;		//callback for processing function

	FuncParamArray	funcParam;	//variable types expected for this function
} FuncType;

typedef vector< vector<FuncType> >		FuncTypeArray;

typedef struct _Function {
	ScriptInd			 type;		//function type (index to look-up function types)
	void				*data;		//sort of 'this' pointer
} Function;

typedef struct _CondDat {
	eCondTest			 condTest;
	Param				 var;
} CondDat;

typedef struct _ScriptLineCond {
	CondDat					dat;

	struct _ScriptLineCond *left;
	struct _ScriptLineCond *right;
} ScriptLineCond;

typedef struct _ScriptLine {
	ScriptLineCond  *condDat;	//condition data
	eCondition		condition;	//type of condition: none, if, else if, else, while

	hFUNC			func;				//function to call
	Param			var;				//this is if we are setting a variable (if func == 0)

	ParamArray	   *params;				//parameters passed in func

	ScriptLinePtrArray *subLines;			//sub lines if there are conditions
	int					 curSubLine;		//-1 if condition hasn't been check yet, if so, SET TO 0!!!!

	RETCODE					prevRet;		//previous line ret., starts with SCRIPTLINERET_DONE
} ScriptLine;

typedef struct _Script {
	VarPtrArray			   *variables;

	ScriptLinePtrArray     *lines;
	int					    curLine;

	RETCODE					prevRet;		//previous line ret., starts with SCRIPTLINERET_DONE
} Script;

typedef struct _PublicParam {
	VarPtrArray			   *variables;	//the 'local' variables from hSCRIPT
	ParamArray			   *params;		//parameters from ScriptLine
} PublicParam;

//main script dude
typedef struct _MainScript {
	VarTypeArray  *varTypes;	//variable types within table of ascii
	FuncTypeArray *funcTypes;  //functions types

	VarPtrArray	  *globalVar;	//global variables
} MainScript;

//internal handle
extern MainScript * g_ScriptTease;

//internal functions

//Script Tease

/////////////////////////////////////
// Name:	ScriptTeaseGetGlobalVarInd
// Purpose:	grabs the global variable
//			index based on name
// Output:	none
// Return:	index, -1 if not found
/////////////////////////////////////
PROTECTED int ScriptTeaseGetGlobalVarInd(const char *name);

/////////////////////////////////////
// Name:	ScriptTeaseGetGlobalVar
// Purpose:	grab global variable
// Output:	none
// Return:	the global variable
/////////////////////////////////////
PROTECTED hVAR ScriptTeaseGetGlobalVar(int ind);

/////////////////////////////////////
// Name:	ScriptTeaseAddGlobalVar
// Purpose:	appends a variable to the
//			global list
// Output:	global var list updated
// Return:	true if added
/////////////////////////////////////
PROTECTED bool ScriptTeaseAddGlobalVar(hVAR var);

//basics

/////////////////////////////////////
// Name:	ScriptBasicInit
// Purpose:	initializes basic
//			variables and functions
//			provided by Script Tease
// Output:	stuff
// Return:	success
/////////////////////////////////////
PROTECTED RETCODE ScriptBasicInit();

#endif