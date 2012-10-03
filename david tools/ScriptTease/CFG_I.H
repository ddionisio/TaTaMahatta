#ifndef _cfg_i_h
#define _cfg_i_h

#include "cfg.h"

#include "..\Parser\parser.h"

typedef struct _CfgItem {
	string		name;		//name of item
	string		val;		//value of item, inside the string
} CfgItem;

typedef list < CfgItem > CfgItemList;

typedef struct _CfgSection {
	string		name;		//section name

	CfgItemList items;		//list of items
} CfgSection;

typedef list < CfgSection > CfgSectionList;

typedef struct _Cfg {
	char			path[MAXCHARBUFF]; //file path
	CfgSectionList	*sections;	//sections
} Cfg;

#endif