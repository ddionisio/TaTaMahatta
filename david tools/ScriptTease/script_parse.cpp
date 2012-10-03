#include "script.h"
#include "script_i.h"

#include "script_variable.h"
#include "script_function.h"
#include "script_line.h"
#include "script_condition.h"

#include "script_parse.h"

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
								  const char *end)
{
	char		buff[MAXCHARBUFF]={0}, lastVarName[MAXCHARBUFF];
	char		retChar;
	ScriptInd	sInd={0};

	bool		bBeginFound = begin ? false : true;
	bool		bEndFound   = end ? false : true;
	bool		bLastGlobal = false;
	int			lastAddedVarInd = 0;
	fpos_t		lastFPos;

	while(1) //go through the file
	{
		fgetpos(fp, &lastFPos);	//used in some places

		//look for a word
		do
		{
			retChar = ParserReadWordFile(fp, buff, MAXCHARBUFF, 0);

			if(strcmp(buff, "\0") != 0)
				break; //we got a word, yay!
		} while(retChar); //there's got to be at least a word in the whole file, so keep looking

		//that means we have reached the end of file
		//or that we reach the end char
		if(retChar == 0)
			break;		 //we are done parsing

		//make sure to look for 'begin' first
		if(!bBeginFound)
		{
			if(strcmp(buff, begin) == 0)
			{
				//begin found
				bBeginFound = true;
				continue; //go on with the loop
			}
		}
		//check to see if it is a comment
		else if(strcmp(buff, "//") == 0)
		{
			//skip the whole line
			ParserSkipCharFile(fp, '\n');
			continue; //go on with the loop
		}
		//check to see if it is a variable type
		else if(ScriptVarGetType(buff, &sInd))
		{
			hVAR newVar = ScriptVarCreate(owner, fp, sInd, false);

			ScriptVarGetName(newVar, lastVarName);

			if(variables) { variables->push_back(newVar); }
		}
		//check to see if we are declaring a 'global' variable
		else if(stricmp(buff, "global") == 0)
		{
			retChar = ParserReadWordFile(fp, buff, MAXCHARBUFF, ' ');

			//check if we reached end of file
			if(retChar == 0)
				break;		 //we are done parsing

			//this has got to be valid
			if(!ScriptVarGetType(buff, &sInd))
			{ ASSERT_MSG(0, "Invalid variable type", "ScriptParseFile"); return RETCODE_FAILURE; }

			hVAR newVar = ScriptVarCreate(owner, fp, sInd, true);

			ScriptVarGetName(newVar, lastVarName);
		}
		//check to see if it is a function type
		else if(ScriptFuncGetType(buff, &sInd))
		{
			ScriptLine * newLine = ScriptLineCreate(owner, fp, variables, sInd,
										false, eScriptLine_Function);

			if(newLine) { lines->push_back(newLine); }
		}
		//check to see if we are setting a variable 'local'
		else if((sInd.ind = ScriptVarGetIndex(buff, variables)) != -1)
		{
			ScriptLine * newLine = ScriptLineCreate(owner, fp, variables, sInd,
										false, eScriptLine_VarSet);

			if(newLine) { lines->push_back(newLine); }
		}
		//check to see if we are setting a variable 'global'
		else if((sInd.ind = ScriptTeaseGetGlobalVarInd(buff)) != -1)
		{
			ScriptLine * newLine = ScriptLineCreate(owner, fp, variables, sInd,
										true, eScriptLine_VarSet);

			if(newLine) { lines->push_back(newLine); }
		}
		//check if it is one of those conditions
		else if((sInd.ind = ScriptCondGetType(buff)) != eCondNone)
		{
			ScriptLine * newLine = ScriptLineCreate(owner, fp, variables, sInd,
										false, eScriptLine_Condition);

			if(newLine) { lines->push_back(newLine); }
		}
		//check if we are initializing a variable, e.g: int i (10)
		else if(buff[0] == '(')
		{
			fsetpos(fp, &lastFPos);	//move back so we can parse it

			ScriptLine * newLine=0;

			//is the last variable local?
			if((sInd.ind = ScriptVarGetIndex(lastVarName, variables)) != -1)
			{
				newLine = ScriptLineCreate(owner, fp, variables, sInd,
											false, eScriptLine_VarSet);
			}
			else if((sInd.ind = ScriptTeaseGetGlobalVarInd(lastVarName)) != -1)
			{
				newLine = ScriptLineCreate(owner, fp, variables, sInd,
											true, eScriptLine_VarSet);
			}
			else
			{ ASSERT_MSG(0, "Unknown crap found!", "ScriptParseFile"); return RETCODE_FAILURE; }

			if(newLine) { lines->push_back(newLine); }
		}
		//check if it is the end
		else if(!bEndFound)
		{
			if(strcmp(buff, end) == 0)
			{
				//end found
				bEndFound = true;
				break;	//done with getting stuff
			}
		}
		//unknown!
		else
		{
			ASSERT_MSG(0, "Unknown crap found!", "ScriptParseFile");
			return RETCODE_FAILURE;
		}
	}

	if(!bBeginFound)
	{ ASSERT_MSG(0, "Begin not found!", "ScriptParseFile"); return RETCODE_FAILURE; }
	else if(!bEndFound)
	{ ASSERT_MSG(0, "End not found!", "ScriptParseFile"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}