#ifndef _script_param_h
#define _script_param_h

//THIS HEADER IS ONLY USED BY SCRIPT TEASE!


/////////////////////////////////////
// Name:	ScriptParamGetItmStr
// Purpose:	grab item from buff with
//			sOut, stops till it reaches endChar
// Output:	sOut
// Return:	true if buff ends
/////////////////////////////////////
PROTECTED bool ScriptParamGetItmStr(char **buff, char *sOut, 
									int sSize, const char endChar);

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
										  Param			    *paramOut);

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
									   ParamArray		*params);

#endif