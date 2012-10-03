#ifndef _cfg_h
#define _cfg_h

//The Config File Loader/Saver

#include "..\common.h"

typedef struct _Cfg	*hCFG;

/////////////////////////////////////
// Name:	CfgFileLoad
// Purpose:	load a config file
// Output:	config file loaded
// Return:	the new config file
/////////////////////////////////////
PUBLIC hCFG CfgFileLoad(const char *filename);

/////////////////////////////////////
// Name:	CfgFileDestroy
// Purpose:	destroy config file
// Output:	cfg destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void CfgFileDestroy(hCFG *cfg);

/////////////////////////////////////
// Name:	CfgFileSave
// Purpose:	save config file
// Output:	file saved from cfg path
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgFileSave(hCFG cfg);

/////////////////////////////////////
// Name:	CfgSectionExist
// Purpose:	check if a section exists
// Output:	none
// Return:	true if section exists
/////////////////////////////////////
PUBLIC bool CfgSectionExist(hCFG cfg, const char *section);

/////////////////////////////////////
// Name:	CfgItemExist
// Purpose:	check if item exists
//			within section
// Output:	none
// Return:	true if item exists
/////////////////////////////////////
PUBLIC bool CfgItemExist(hCFG cfg, const char *section, const char *item);

/////////////////////////////////////
// Name:	CfgGetItemStr
// Purpose:	get item string
// Output:	sOut is filled
// Return:	true if found and set given
//			string
/////////////////////////////////////
PUBLIC bool CfgGetItemStr(hCFG cfg, const char *section, const char *item, char *sOut);

/////////////////////////////////////
// Name:	CfgGetItemInt
// Purpose:	get item int
// Output:	none
// Return:	-1 if item does not exists...
//			but hey, maybe you did want -1
/////////////////////////////////////
PUBLIC int CfgGetItemInt(hCFG cfg, const char *section, const char *item);

/////////////////////////////////////
// Name:	CfgGetItemFloat
// Purpose:	get item float
// Output:	none
// Return:	-1 if item does not exists...
//			but hey, maybe you did want -1
/////////////////////////////////////
PUBLIC float CfgGetItemFloat(hCFG cfg, const char *section, const char *item);

/////////////////////////////////////
// Name:	CfgAddItemStr
// Purpose:	add or set item string
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgAddItemStr(hCFG cfg, const char *section, const char *item, char *str);

/////////////////////////////////////
// Name:	CfgAddItemInt
// Purpose:	add or set item int
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgAddItemInt(hCFG cfg, const char *section, const char *item, int val);

/////////////////////////////////////
// Name:	CfgAddItemFloat
// Purpose:	add or set item float
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgAddItemFloat(hCFG cfg, const char *section, const char *item, float val);

#endif