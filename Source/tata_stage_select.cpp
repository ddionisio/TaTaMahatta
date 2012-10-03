#include "tata_main.h"

#include "tata_globals.h"

#include "tata_stage_select.h"

/*
ST_LOCK,
	ST_UNLOCK,
	ST_DONE
} eSelectType;*/

//Anything that has got to do with stage select

struct SelectLevel {
	eSelectType type;	//ST_LOCK, ST_UNLOCK, ST_DONE (based on player profile)
	D3DXVECTOR2 sCLoc;	//center location (in pixel)

	char		levelName[MAXCHARBUFF];	//level name
	char		mapFile[MAXCHARBUFF];	//map file
};

struct SelectStage {
	eSelectType type;					//ST_LOCK, ST_UNLOCK (based on player profile)
	hTXT		mapImg;					//image of map

	vector<SelectLevel> levels;	//the levels
};

vector<SelectStage> g_stages;

/////////////////////////////////////
// Name:	StageSelectDestroy
// Purpose:	destroys the stage select
//			stuff, call this after done
//			with game
// Output:	stages destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void StageSelectDestroy()
{
	for(int i = 0; i < g_stages.size(); i++)
	{
		if(g_stages[i].mapImg)
			TextureDestroy(&g_stages[i].mapImg);
	}

	g_stages.clear();
}

PRIVATE void _StageSelectLoadItems(hCFG stageCfg, hCFG playerProfile,
								   const char *section, SelectStage *pStage)
{
	int level = 0;
	char levelStr[MAXCHARBUFF]={0}, levelValStr[MAXCHARBUFF]={0}, 
		lvlMapPath[MAXCHARBUFF]={0}, *pBuff;

	SIZE scrnSize = GFXGetScrnSize();

	while(1)
	{
		sprintf(levelStr, "level_%d", level);

		if(CfgItemExist(stageCfg, section, levelStr))
		{
			if(CfgGetItemStr(stageCfg, section, levelStr, levelValStr))
			{
				SelectLevel newLevel;

				//get mapfilename
				pBuff = levelValStr;
				if(ParserReadStringBuff(&pBuff, lvlMapPath, MAXCHARBUFF, '"', '"')
					== RETCODE_ENDREACHED)
				{
					string txtFile = GAMEFOLDER;
					txtFile += "\\";
					txtFile += lvlMapPath;

					strcpy(newLevel.mapFile, txtFile.c_str());
				}

				//get name
				ParserSkipCharBuff(&pBuff, ',');
				ParserReadStringBuff(&pBuff, newLevel.levelName, MAXCHARBUFF, '"', '"');

				//get relative location
				D3DXVECTOR2 relLoc;
				sscanf(pBuff, ",%f,%f", &relLoc.x, &relLoc.y);

				//center location
				newLevel.sCLoc.x = relLoc.x*scrnSize.cx;
				newLevel.sCLoc.y = relLoc.y*scrnSize.cy;

				//determine state from profile
				if(playerProfile && CfgItemExist(playerProfile, section, levelStr))
				{
					if(CfgGetItemInt(playerProfile, section, levelStr) == -1)
						newLevel.type = ST_LOCK;
					else
						newLevel.type = (eSelectType)CfgGetItemInt(playerProfile, section, levelStr);
				}
				else
					newLevel.type = ST_LOCK;

				//add inside stage
				pStage->levels.push_back(newLevel);
			}

			level++;
		}
		else
			break;
	}
}

/////////////////////////////////////
// Name:	StageSelectInit
// Purpose:	initialize the stage and
//			level lists, call this after
//			loading a profile
// Output:	stages initialized
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE StageSelectInit(const char *levelFile, hCFG playerProfile)
{
	TataDisplayLoadText("Destroying Previous Stages...");
	StageSelectDestroy();

	hCFG stageCfg = CfgFileLoad(levelFile);

	int stage = 0;
	char stageStr[MAXCHARBUFF]={0}, mapImgPath[MAXCHARBUFF]={0};

	string txtFile;

	if(stageCfg)
	{
		//go find all the stages
		while(1)
		{
			sprintf(stageStr, "stage_%d", stage);

			if(CfgSectionExist(stageCfg, stageStr))
			{
				TataDisplayLoadText("Parsing Stage %d", stage);

				SelectStage newStage;

				//get map image file
				if(CfgGetItemStr(stageCfg, stageStr, "mapImage", mapImgPath))
				{
					txtFile = GAMEFOLDER;
					txtFile += "\\";
					txtFile += mapImgPath;

					newStage.mapImg = TextureCreate(0, txtFile.c_str(), false, 0);
				}
				else
					newStage.mapImg = 0;

				//determine if locked or unlocked
				if(playerProfile && CfgSectionExist(playerProfile, stageStr))
					newStage.type = ST_UNLOCK;
				else
					newStage.type = ST_LOCK;

				//load the items
				_StageSelectLoadItems(stageCfg, playerProfile, stageStr, &newStage);

				//add to global stages
				g_stages.push_back(newStage);

				stage++;
			}
			else
				break;
		}

		CfgFileDestroy(&stageCfg);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	StageSelectDisplay
// Purpose:	display the stage with the
//			dots and everything...
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE StageSelectDisplay(int stage, const D3DXVECTOR2 & mLoc,
								  const D3DXVECTOR2 & mSize, hTXT dot[ST_MAX],
								  float dotSize, DWORD clr)
{
	if(stage >= 0 && stage < g_stages.size() && g_stages[stage].mapImg)
	{
		//blt the map
		TextureStretchBlt(g_stages[stage].mapImg, mLoc.x, mLoc.y,
			mSize.x, mSize.y, 0, &clr, 0);

		//blt the dots
		for(int i = 0; i < g_stages[stage].levels.size(); i++)
		{
			float x = g_stages[stage].levels[i].sCLoc.x - (dotSize/2);
			float y = g_stages[stage].levels[i].sCLoc.y - (dotSize/2);

			if(dot[g_stages[stage].levels[i].type])
				TextureStretchBlt(dot[g_stages[stage].levels[i].type], 
					x, y, dotSize, dotSize, 0, &clr, 0);
		}
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	StageSelectGetLevelLoc
// Purpose:	get level location
// Output:	none
// Return:	the location
/////////////////////////////////////
PUBLIC D3DXVECTOR2 StageSelectGetLevelLoc(int stage, int level)
{
	if(stage >= 0 && stage < g_stages.size() && level < g_stages[stage].levels.size())
		return g_stages[stage].levels[level].sCLoc;

	return D3DXVECTOR2(0,0);
}

/////////////////////////////////////
// Name:	StageSelectGetLevelMap
// Purpose:	get the map file from level
// Output:	none
// Return:	the map file
/////////////////////////////////////
PUBLIC const char * StageSelectGetLevelMap(int stage, int level)
{
	if(stage >= 0 && stage < g_stages.size() && level < g_stages[stage].levels.size())
		return g_stages[stage].levels[level].mapFile;

	return 0;
}

/////////////////////////////////////
// Name:	StageSelectGetNumStage
// Purpose:	get number of stages
// Output:	none
// Return:	number of stages
/////////////////////////////////////
PUBLIC unsigned int StageSelectGetNumStage()
{
	return g_stages.size();
}

/////////////////////////////////////
// Name:	StageSelectGetNumLevels
// Purpose:	get number of levels in
//			a given stage
// Output:	none
// Return:	number of levels
/////////////////////////////////////
PUBLIC unsigned int StageSelectGetNumLevels(int stage)
{
	if(stage >= 0 && stage < g_stages.size())
		return g_stages[stage].levels.size();

	return 0;
}

/////////////////////////////////////
// Name:	StageSelectGetStageState
// Purpose:	get the state of stage
// Output:	none
// Return:	state of stage
/////////////////////////////////////
PUBLIC eSelectType StageSelectGetStageState(int stage)
{
	if(stage < 0 || stage >= g_stages.size())
		return ST_LOCK;

	return g_stages[stage].type;
}

/////////////////////////////////////
// Name:	StageSelectGetLevelState
// Purpose:	get the state of level
// Output:	none
// Return:	state of level
/////////////////////////////////////
PUBLIC eSelectType StageSelectGetLevelState(int stage, int level)
{
	if(stage < 0 || stage >= g_stages.size() 
		|| level < 0 || level >= g_stages[stage].levels.size())
		return ST_LOCK;

	return g_stages[stage].levels[level].type;
}

/////////////////////////////////////
// Name:	StageSelectGetLevelName
// Purpose:	get the level name
// Output:	none
// Return:	the name
/////////////////////////////////////
PUBLIC const char * StageSelectGetLevelName(int stage, int level)
{
	if(stage >= 0 && stage < g_stages.size() && level < g_stages[stage].levels.size())
		return g_stages[stage].levels[level].levelName;

	return 0;
}

/////////////////////////////////////
// Name:	StageSelectUnlockNext
// Purpose:	unlock the next level based
//			on given stage,level.
//			the given level will be
//			set to done.
// Output:	profile set, as well as the
//			stage data
// Return:	true if able to unlock the
//			next level/stage.
//			false means that all levels
//			have been completed
/////////////////////////////////////
PUBLIC bool StageSelectUnlockNext(hCFG playerProfile, int stage, int level)
{
	if(!playerProfile)
		return true;

	//CfgAddItemInt(hCFG cfg, const char *section, const char *item, int val)
	bool ret=false;

	if(stage >= 0 && stage < g_stages.size())
	{
		if(level < g_stages[stage].levels.size())
		{
			char stageStr[MAXCHARBUFF], levelStr[MAXCHARBUFF];

			sprintf(stageStr, "stage_%d", stage);
			sprintf(levelStr, "level_%d", level);

			//set the current level to done
			CfgAddItemInt(playerProfile, stageStr, levelStr, ST_DONE);

			g_stages[stage].levels[level].type = ST_DONE;

			//now for the next stage/level
			int stageNext=stage, levelNext=level+1;

			if(levelNext >= g_stages[stage].levels.size())
			{ stageNext++; levelNext=0; }

			//there better be a stage next, otherwise
			//player has completed all the levels
			//also, only unlock if level is locked.
			StageSelectUnlock(playerProfile, stageNext, levelNext);

			g_level = levelNext;
		}
	}

	return ret;
}

/////////////////////////////////////
// Name:	StageSelectUnlock
// Purpose:	unlock given level/stage
// Output:	unlocked
// Return:	none
/////////////////////////////////////
PUBLIC void StageSelectUnlock(hCFG playerProfile, int stage, int level)
{
	if(stage >= 0 && stage < g_stages.size())
	{
		g_stages[stage].type = ST_UNLOCK;

		if(level < g_stages[stage].levels.size())
		{
			//set the current level to unlock
			if(g_stages[stage].levels[level].type == ST_LOCK)
			{
				char stageStr[MAXCHARBUFF], levelStr[MAXCHARBUFF];

				sprintf(stageStr, "stage_%d", stage);
				sprintf(levelStr, "level_%d", level);

				CfgAddItemInt(playerProfile, stageStr, levelStr, ST_UNLOCK);
				g_stages[stage].levels[level].type = ST_UNLOCK;
			}
		}
	}
}

/////////////////////////////////////
// Name:	StageSelectAllLevelDone
// Purpose:	check to see if all the
//			levels in a stage are
//			done.
// Output:	none
// Return:	true if all levels are done
/////////////////////////////////////
PUBLIC bool StageSelectAllLevelDone(int stage)
{
	int numDone = 0;

	if(stage >= 0 && stage < g_stages.size())
	{
		for(int i = 0; i < g_stages[stage].levels.size(); i++)
			if(g_stages[stage].levels[i].type == ST_DONE)
				numDone++;
	}

	if(numDone == g_stages[stage].levels.size())
		return true;

	return false;
}

/////////////////////////////////////
// Name:	StageSelectLoadData
// Purpose:	load the level data
// Output:	dat filled
// Return:	true=data exists
/////////////////////////////////////
PUBLIC bool StageSelectLoadData(hCFG playerProfile, int stage, int level, 
								StageDat & dat)
{
	char stageStr[MAXCHARBUFF], levelStr[MAXCHARBUFF], buff[MAXCHARBUFF]={0};

		sprintf(stageStr, "stage_%d", stage);
		sprintf(levelStr, "level_%d_dat", level);

	if(CfgGetItemStr(playerProfile, stageStr, levelStr, buff))
	{
		sscanf(buff, "%d,%d,%d,%d,%d", &dat.hr, &dat.min, &dat.sec, 
			&dat.artafactMin, &dat.artafactMax);

		return true;
	}
	else
		memset(&dat, 0, sizeof(dat));

	return false;
}

/////////////////////////////////////
// Name:	StageSelectSaveData
// Purpose:	save the level data
//			from world
// Output:	profile set
// Return:	none
/////////////////////////////////////
PUBLIC void StageSelectSaveData(hCFG playerProfile, int stage, int level)
{
	if(playerProfile && g_world && stage >= 0 && stage < g_stages.size())
	{
		int bestHr=g_world->GetClock().hour, 
			bestMin=g_world->GetClock().min,
			bestSec=g_world->GetClock().sec;

		char stageStr[MAXCHARBUFF], levelStr[MAXCHARBUFF], buff[MAXCHARBUFF]={0};

		sprintf(stageStr, "stage_%d", stage);
		sprintf(levelStr, "level_%d_dat", level);

		StageDat dat;

		//if item already exists, get best of time
		if(StageSelectLoadData(playerProfile, stage, level, dat))
		{
			if(dat.hr < bestHr)
			{
				bestHr = dat.hr; bestMin = dat.min; bestSec = dat.sec;
			}
			else if(dat.hr == bestHr)
			{
				if(dat.min < bestMin)
				{
					bestMin = dat.min; bestSec = dat.sec;
				}
				else if(dat.min == bestMin)
				{
					if(dat.sec < bestSec)
						bestSec = dat.sec;
				}
			}
		}

		//set data
		sprintf(buff, "%d,%d,%d,%d,%d", bestHr, bestMin, bestSec, 
			g_world->GetArTaFactMin(), g_world->GetArTaFactMax());

		CfgAddItemStr(playerProfile, stageStr, levelStr, buff);
	}
}