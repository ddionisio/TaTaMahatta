#include "cfg.h"
#include "cfg_i.h"

#include "script_i.h"

PRIVATE bool _CfgIsSection(char *buff)
{
	if(buff[0] == '[' && buff[strlen(buff)-1] == ']')
		return true;

	return false;
}

PRIVATE void _CfgGetItem(char *buff, hCFG cfg)
{
	char *buffWalker = buff;

	char itmBuff[MAXCHARBUFF];

	CfgItem newItem;

	//get item
	if(ParserReadWordBuff(&buffWalker, itmBuff, MAXCHARBUFF, '=') == '=')
	{
		newItem.name = itmBuff;

		//get the rest of the item
		newItem.val = buffWalker;

		cfg->sections->back().items.push_back(newItem);
	}
}

PRIVATE RETCODE _CfgLoadSectionsNItems(FILE *fp, hCFG cfg)
{
	char *buff;
	int   buffSize;

	RETCODE ret;

	while(!feof(fp))
	{
		ret = ParserCountStringFile(fp, &buffSize, 0, '\n');

		if(ret == RETCODE_FAILURE || buffSize <= 0) //something bad
			break;

		buffSize++;

		buff = (char*)SCRIPT_MALLOC(sizeof(char)*buffSize);

		ParserReadStringFile(fp, buff, buffSize, 0, '\n');

		//determine if it is a section, just check the two ends for '[' and ']'
		if(_CfgIsSection(buff))
		{
			char *stuffBuff = buff;
			
			stuffBuff[buffSize-2] = 0; //remove the '[' and ']'
			stuffBuff++;

			CfgSection newSection;

			newSection.name = stuffBuff;

			cfg->sections->push_back(newSection);
		}
		//it's an item, attempt to get the item if a section exists
		//if so, get the last
		else if(cfg->sections->size() > 0)
		{
			_CfgGetItem(buff, cfg);
		}

		SCRIPT_FREE(buff);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	CfgFileLoad
// Purpose:	load a config file
// Output:	config file loaded
// Return:	the new config file
/////////////////////////////////////
PUBLIC hCFG CfgFileLoad(const char *filename)
{
	hCFG newConfig = 0;

	newConfig = (hCFG)SCRIPT_MALLOC(sizeof(Cfg));

	newConfig->sections = new CfgSectionList; assert(newConfig->sections);

	strcpy(newConfig->path, filename);

	FILE *fp = fopen(filename, "rt");

	if(fp)
	{
		_CfgLoadSectionsNItems(fp, newConfig);

		fclose(fp);
	}

	return newConfig;
}

/////////////////////////////////////
// Name:	CfgFileDestroy
// Purpose:	destroy config file
// Output:	cfg destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void CfgFileDestroy(hCFG *cfg)
{
	if(*cfg)
	{
		if((*cfg)->sections)
			delete (*cfg)->sections;

		SCRIPT_FREE(*cfg);

		cfg = 0;
	}
}

/////////////////////////////////////
// Name:	CfgFileSave
// Purpose:	save config file
// Output:	file saved from cfg path
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgFileSave(hCFG cfg)
{
	FILE *fp = fopen(cfg->path, "wt");

	if(!fp) { return RETCODE_FAILURE; }

	for(CfgSectionList::iterator i =  cfg->sections->begin(); i != cfg->sections->end(); i++)
	{
		//write the section
		fprintf(fp, "[%s]\n", (*i).name.c_str());

		for(CfgItemList::iterator j = (*i).items.begin(); j != (*i).items.end(); j++)
		{
			//write item and it's value
			fprintf(fp, "%s=%s\n", (*j).name.c_str(), (*j).val.c_str());
		}
	}

	fclose(fp);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	CfgSectionExist
// Purpose:	check if a section exists
// Output:	none
// Return:	true if section exists
/////////////////////////////////////
PUBLIC bool CfgSectionExist(hCFG cfg, const char *section)
{
	for(CfgSectionList::iterator i =  cfg->sections->begin(); i != cfg->sections->end(); i++)
	{
		if(strcmp((*i).name.c_str(), section) == 0)
			return true;
	}

	return false;
}

/////////////////////////////////////
// Name:	CfgItemExist
// Purpose:	check if item exists
//			within section
// Output:	none
// Return:	true if item exists
/////////////////////////////////////
PUBLIC bool CfgItemExist(hCFG cfg, const char *section, const char *item)
{
	for(CfgSectionList::iterator i =  cfg->sections->begin(); i != cfg->sections->end(); i++)
	{
		if(strcmp((*i).name.c_str(), section) == 0)
		{
			for(CfgItemList::iterator j = (*i).items.begin(); j != (*i).items.end(); j++)
			{
				if(strcmp((*j).name.c_str(), item) == 0)
					return true;
			}
		}
	}

	return false;
}

/////////////////////////////////////
// Name:	CfgGetItemStr
// Purpose:	get item string
// Output:	sOut is filled
// Return:	true if found and set given
//			string
/////////////////////////////////////
PUBLIC bool CfgGetItemStr(hCFG cfg, const char *section, const char *item, char *sOut)
{
	for(CfgSectionList::iterator i =  cfg->sections->begin(); i != cfg->sections->end(); i++)
	{
		//match section
		if(strcmp((*i).name.c_str(), section) == 0)
		{
			for(CfgItemList::iterator j = (*i).items.begin(); j != (*i).items.end(); j++)
			{
				//match item
				if(strcmp((*j).name.c_str(), item) == 0)
				{
					strcpy(sOut, (*j).val.c_str());

					return true;
				}
			}
		}
	}

	return false;
}

/////////////////////////////////////
// Name:	CfgGetItemInt
// Purpose:	get item int
// Output:	none
// Return:	-1 if item does not exists...
//			but hey, maybe you did want -1
/////////////////////////////////////
PUBLIC int CfgGetItemInt(hCFG cfg, const char *section, const char *item)
{
	char buff[MAXCHARBUFF]={0};

	if(CfgGetItemStr(cfg, section, item, buff))
	{
		int i;

		sscanf(buff, "%d", &i);

		return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	CfgGetItemFloat
// Purpose:	get item float
// Output:	none
// Return:	-1 if item does not exists...
//			but hey, maybe you did want -1
/////////////////////////////////////
PUBLIC float CfgGetItemFloat(hCFG cfg, const char *section, const char *item)
{
	char buff[MAXCHARBUFF]={0};

	if(CfgGetItemStr(cfg, section, item, buff))
	{
		float f;

		sscanf(buff, "%f", &f);

		return f;
	}

	return -1;
}

/////////////////////////////////////
// Name:	CfgAddItemStr
// Purpose:	add or set item string
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgAddItemStr(hCFG cfg, const char *section, const char *item, char *str)
{
	CfgItem newItem;

	newItem.name = item;
	newItem.val  = str;

	for(CfgSectionList::iterator i =  cfg->sections->begin(); i != cfg->sections->end(); i++)
	{
		//match section
		if(strcmp((*i).name.c_str(), section) == 0)
		{
			for(CfgItemList::iterator j = (*i).items.begin(); j != (*i).items.end(); j++)
			{
				//match item
				if(strcmp((*j).name.c_str(), item) == 0)
				{
					(*j).val = str;

					return RETCODE_SUCCESS;
				}
			}

			//add new item
			(*i).items.push_back(newItem);

			return RETCODE_SUCCESS;
		}
	}

	//create new section if section does not exists
	CfgSection newSection;

	newSection.name = section;
	newSection.items.push_back(newItem);

	cfg->sections->push_back(newSection);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	CfgAddItemInt
// Purpose:	add or set item int
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgAddItemInt(hCFG cfg, const char *section, const char *item, int val)
{
	char buff[MAXCHARBUFF];

	sprintf(buff, "%d", val);

	return CfgAddItemStr(cfg, section, item, buff);
}

/////////////////////////////////////
// Name:	CfgAddItemFloat
// Purpose:	add or set item float
//			will add item if given item
//			name does not exists
// Output:	modify or add item
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE CfgAddItemFloat(hCFG cfg, const char *section, const char *item, float val)
{
	char buff[MAXCHARBUFF];

	sprintf(buff, "%f", val);

	return CfgAddItemStr(cfg, section, item, buff);
}