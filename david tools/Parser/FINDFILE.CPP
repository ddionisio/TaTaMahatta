#include <io.h>

#include "parser.h"
#include "i_parser.h"

#define BIG_BUFFERSIZE 1024

/////////////////////////////////////
// Name:	ParserFindFiles
// Purpose:	get all files in given directory
//			with pattern and process it
// Output:	stuff
// Return:	none
/////////////////////////////////////
PUBLIC void ParserFindFiles(char *dirname, char *pattern, bool bCheckSubDir, FINDCALLBACK callback, void *userDat)
{
	char big_buffer[BIG_BUFFERSIZE];
	char filePath[BIG_BUFFERSIZE];

	long file;		//this is for the findfirst argument
	struct _finddata_t stuff; //the structure of finddata_t for use of stuff
	int arefiles;
	
	if(strlen(dirname) + strlen(pattern) + 2 > BIG_BUFFERSIZE) // why do I add 2 here???
	{														   // because of the backslash!!!
		printf("ERROR - path too long!!!\n");
		exit(1);
	}
	strcpy(big_buffer, dirname);
	strcat(big_buffer, "\\");
	strcat(big_buffer, pattern);

    // INITIALIZATION STARTS HERE (file listing and such)
	file = _findfirst( big_buffer, &stuff );



	if( (file = _findfirst( big_buffer, &stuff )) != -1L ) //print the files if there are any
	{
	   do
	   {
		   if ((strcmp(stuff.name, ".") == 0) || (strcmp(stuff.name, "..") == 0))
			   continue; // ignore both dots...

		   //let the user process it
		   if(!( stuff.attrib & _A_SUBDIR))
		   {
			    strcpy(filePath, dirname);
				strcat(filePath, "\\");
				strcat(filePath, stuff.name);
			if(callback)
				callback(filePath, userDat);
		   }

	   }while( _findnext( file, &stuff ) == 0 );
	   
	}

//CHECK FOR OTHER DIR------------------------------------------------------------
	if(bCheckSubDir)
	{
		strcpy(big_buffer, dirname);
		strcat(big_buffer, "\\");
		strcat(big_buffer, "*"); //star means check all files in current DIR

		arefiles = 1; //assume that there are files

		if( (file = _findfirst( big_buffer, &stuff )) == -1L ) //if it is not a file
			arefiles = 0;
		else if(_findnext( file, &stuff ) == -1L) // If you don't skip twice, there will be an endless loop
			arefiles = 0;

		if(arefiles) //same as above after the two dots, if the next file after the 2 dot is a file 
		{
			while( _findnext( file, &stuff ) != -1L) //look for more directories...while there is something
			{
				if( stuff.attrib & _A_SUBDIR)    
					{
						strcpy(big_buffer,dirname);
						strcat(big_buffer,"\\");
						strcat(big_buffer,stuff.name);

						//recall the function with new directory  
						ParserFindFiles(big_buffer, pattern, bCheckSubDir, callback, userDat);
					}  
			}
		}
	}
	
	   _findclose( file );
}