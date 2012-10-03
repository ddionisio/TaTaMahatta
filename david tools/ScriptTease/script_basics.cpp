#include "script.h"
#include "script_i.h"

#include "script_basics.h"

//This will initialize all basic variables and functions
//provided by Script Tease

/////////////////////////////////////
// Name:	ScriptBasicInit
// Purpose:	initializes basic
//			variables and functions
//			provided by Script Tease
// Output:	stuff
// Return:	success
/////////////////////////////////////
PROTECTED RETCODE ScriptBasicInit()
{
	//initialize variables
	ScriptTeaseAddVarType("int", sizeof(int), "int", IntCallback);
	ScriptTeaseAddVarType("float", sizeof(int), "float", FloatCallback);
	ScriptTeaseAddVarType("string", sizeof(char *), "string", StringCallback);
	ScriptTeaseAddVarType("file", sizeof(tFile), "string", FileCallback);

	ScriptTeaseAddVarType("vector", sizeof(float)*2, "float, float", VectorCallback);

	//initialize functions
	ScriptTeaseAddFuncType("inc_i", "int", inc_iCallback);
	ScriptTeaseAddFuncType("dec_i", "int", dec_iCallback);
	ScriptTeaseAddFuncType("add_i", "int, int, int", add_iCallback);
	ScriptTeaseAddFuncType("sub_i", "int, int, int", sub_iCallback);
	ScriptTeaseAddFuncType("mult_i", "int, int, int", mult_iCallback);
	ScriptTeaseAddFuncType("div_i", "int, int, int", div_iCallback);
	ScriptTeaseAddFuncType("mod_i", "int, int, int", mod_iCallback);
	ScriptTeaseAddFuncType("random_i", "int, int, int", random_iCallback);

	ScriptTeaseAddFuncType("inc_f", "float", inc_fCallback);
	ScriptTeaseAddFuncType("dec_f", "float", dec_fCallback);
	ScriptTeaseAddFuncType("add_f", "float, float, float", add_fCallback);
	ScriptTeaseAddFuncType("sub_f", "float, float, float", sub_fCallback);
	ScriptTeaseAddFuncType("mult_f", "float, float, float", mult_fCallback);
	ScriptTeaseAddFuncType("div_f", "float, float, float", div_fCallback);
	ScriptTeaseAddFuncType("sin", "float, float", sinCallback);
	ScriptTeaseAddFuncType("cos", "float, float", cosCallback);
	ScriptTeaseAddFuncType("tan", "float, float", tanCallback);
	ScriptTeaseAddFuncType("asin", "float, float", asinCallback);
	ScriptTeaseAddFuncType("acos", "float, float", acosCallback);
	ScriptTeaseAddFuncType("atan", "float, float", atanCallback);
	ScriptTeaseAddFuncType("sqrt", "float, float", sqrtCallback);
	ScriptTeaseAddFuncType("degtorad", "float, float", degtoradCallback);
	ScriptTeaseAddFuncType("radtodeg", "float, float", radtodegCallback);
	ScriptTeaseAddFuncType("random_f", "float, float, float", random_fCallback);

	ScriptTeaseAddFuncType("strcat", "string, string", strcatCallback);

	ScriptTeaseAddFuncType("file_print_int", "file, int", file_print_intCallback);
	ScriptTeaseAddFuncType("file_print_float", "file, float", file_print_floatCallback);
	ScriptTeaseAddFuncType("file_print_string", "file, string", file_print_stringCallback);

	return RETCODE_SUCCESS;
}