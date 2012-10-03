#ifndef _script_parse_h
#define _script_parse_h

//THIS HEADER IS ONLY USED BY SCRIPT TEASE!

/////////////////////////////////////
// Name:	ScriptParseFile
// Purpose:	parses the file and fills the
//			given variables and lines
//			this might also fill up the
//			global variables
// Output:	variables and lines are filled,
//			also global variables if any
// Return:	success if success
/////////////////////////////////////
PROTECTED RETCODE ScriptParseFile(void *owner,
								  FILE * fp, 
								  VarPtrArray *variables,
								  ScriptLinePtrArray *lines,
								  const char *begin, 
								  const char *end);

#endif