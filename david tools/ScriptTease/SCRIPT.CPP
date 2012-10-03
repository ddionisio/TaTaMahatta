#include "script.h"
#include "script_i.h"

#include "script_variable.h"
#include "script_parse.h"
#include "script_line.h"

/////////////////////////////////////
// Name:	ScriptLoad
// Purpose:	loads a script (tons of shit happens here)
//			NOTE: some variables/functions may
//				  require that you put something in owner
//				  beware that some will expect the 'correct owner'
// Output:	script loaded
// Return:	the script
/////////////////////////////////////
PUBLIC hSCRIPT ScriptLoad(void *owner, const char *filename, const char *begin, const char *end)
{
	assert(g_ScriptTease); //this should first be initialized

	hSCRIPT newScript = (hSCRIPT)SCRIPT_MALLOC(sizeof(Script));

	if(!newScript) { ASSERT_MSG(0, "Unable to allocate new script!", "ScriptLoad"); return 0; }

	newScript->prevRet	 = SCRIPTLINERET_DONE;
	newScript->variables = new VarPtrArray;
	newScript->lines	 = new ScriptLinePtrArray;

	FILE *fp = fopen(filename, "rt");

	if(!fp) { ASSERT_MSG(0, "Unable to open file!", "ScriptLoad"); ScriptDestroy(owner, &newScript); return 0; }

	if(ScriptParseFile(owner, fp, newScript->variables, newScript->lines, begin, end) != RETCODE_SUCCESS)
	{ ScriptDestroy(owner, &newScript); return 0; }

	fclose(fp);

	return newScript;
}

/////////////////////////////////////
// Name:	ScriptDestroy
// Purpose:	destroys a script
//			NOTE: some may require the owner
// Output:	script ded
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptDestroy(void *owner, hSCRIPT *script)
{
	if(*script)
	{
		if((*script)->lines)
		{
			for(int i = 0; i < (*script)->lines->size(); i++)
				ScriptLineDestroy(owner, &(*(*script)->lines)[i]);

			delete (*script)->lines;
		}

		if((*script)->variables)
		{
			for(int i = 0; i < (*script)->variables->size(); i++)
				ScriptVarDestroy(owner, &(*(*script)->variables)[i]);

			delete (*script)->variables;
		}

		SCRIPT_FREE(*script);

		script = 0;
	}
}

/////////////////////////////////////
// Name:	ScriptUpdate
// Purpose:	updates script until done
// Output:	script update
// Return:	RETCODE_BREAK if done
/////////////////////////////////////
PUBLIC RETCODE ScriptUpdate(void *owner, hSCRIPT script)
{
	if(script->curLine < script->lines->size())
	{
		script->prevRet = ScriptLineExecute(owner,
								 (*script->lines)[script->curLine],
								 script->variables,
								 script->prevRet);

		if(script->prevRet == SCRIPTLINERET_DONE
			|| script->prevRet == SCRIPTLINERET_CONDITIONFAIL)
			script->curLine++;
	}
	else
		return RETCODE_BREAK;	//we are done...

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	ScriptReset
// Purpose:	reset the script back to the
//			first line of code
// Output:	current line back to 0
// Return:	none
/////////////////////////////////////
PUBLIC void ScriptReset(hSCRIPT script)
{
	script->curLine = 0;

	script->prevRet = SCRIPTLINERET_DONE;

	for(int i = 0; i < script->lines->size(); i++)
		ScriptLineReset((*script->lines)[i]);
}