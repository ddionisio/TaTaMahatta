#include "tata_main.h"

//A simple error log interface for the game

typedef struct _errorDat {
	string header;		//title of error (usu. function name)
	string msg;			//error message
} errorDat;

list<errorDat> g_errorList;

/////////////////////////////////////
// Name:	ErrorMsg
// Purpose:	add/display error
// Output:	error added
// Return:	none
/////////////////////////////////////
PUBLIC void ErrorMsg(const char *header, const char *msg, bool bAssert)
{
	errorDat newError;

	newError.header = header;
	newError.msg    = msg;

	g_errorList.push_back(newError);

	if(bAssert)
		ASSERT_MSG(0, header, msg);
}

/////////////////////////////////////
// Name:	ErrorDump
// Purpose:	dump error to file
// Output:	error dump to file
// Return:	none
/////////////////////////////////////
PUBLIC void ErrorDump(const char *filename)
{
	if(g_errorList.size() == 0) return;

	FILE *fp = fopen(filename, "wt");

	if(fp)
	{
		for(list<errorDat>::iterator i = g_errorList.begin(); i != g_errorList.end(); i++)
			fprintf(fp, "<%s>: %s\n", (*i).header.c_str(), (*i).msg.c_str());

		fclose(fp);
	}
}