#ifndef _tata_stage_select_h
#define _tata_stage_select_h

typedef enum {
	ST_UNLOCK,
	ST_DONE,
	ST_LOCK,
	ST_MAX
} eSelectType;

typedef struct _StageDat {
	int hr,min,sec;					//the best time
	int artafactMin,artafactMax;	//Ar-Ta-Fact collected
} StageDat;

/////////////////////////////////////
// Name:	StageSelectInit
// Purpose:	initialize the stage and
//			level lists, call this after
//			loading a profile
// Output:	stages initialized
// Return:	success if success
/////////////////////////////////////
PUBLIC RETCODE StageSelectInit(const char *levelFile, hCFG playerProfile);

/////////////////////////////////////
// Name:	StageSelectDestroy
// Purpose:	destroys the stage select
//			stuff, call this after done
//			with game
// Output:	stages destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void StageSelectDestroy();

/////////////////////////////////////
// Name:	StageSelectDisplay
// Purpose:	display the stage with the
//			dots and everything...
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE StageSelectDisplay(int stage, const D3DXVECTOR2 & mLoc,
								  const D3DXVECTOR2 & mSize, hTXT dot[ST_MAX],
								  float dotSize, DWORD clr);

/////////////////////////////////////
// Name:	StageSelectGetLevelLoc
// Purpose:	get level location
// Output:	none
// Return:	the location
/////////////////////////////////////
PUBLIC D3DXVECTOR2 StageSelectGetLevelLoc(int stage, int level);

/////////////////////////////////////
// Name:	StageSelectGetLevelMap
// Purpose:	get the map file from level
// Output:	none
// Return:	the map file
/////////////////////////////////////
PUBLIC const char * StageSelectGetLevelMap(int stage, int level);

/////////////////////////////////////
// Name:	StageSelectGetNumStage
// Purpose:	get number of stages
// Output:	none
// Return:	number of stages
/////////////////////////////////////
PUBLIC unsigned int StageSelectGetNumStage();

/////////////////////////////////////
// Name:	StageSelectGetNumLevels
// Purpose:	get number of levels in
//			a given stage
// Output:	none
// Return:	number of levels
/////////////////////////////////////
PUBLIC unsigned int StageSelectGetNumLevels(int stage);

/////////////////////////////////////
// Name:	StageSelectGetStageState
// Purpose:	get the state of stage
// Output:	none
// Return:	state of stage
/////////////////////////////////////
PUBLIC eSelectType StageSelectGetStageState(int stage);

/////////////////////////////////////
// Name:	StageSelectGetLevelState
// Purpose:	get the state of level
// Output:	none
// Return:	state of level
/////////////////////////////////////
PUBLIC eSelectType StageSelectGetLevelState(int stage, int level);

/////////////////////////////////////
// Name:	StageSelectGetLevelName
// Purpose:	get the level name
// Output:	none
// Return:	the name
/////////////////////////////////////
PUBLIC const char * StageSelectGetLevelName(int stage, int level);

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
PUBLIC bool StageSelectUnlockNext(hCFG playerProfile, int stage, int level);

/////////////////////////////////////
// Name:	StageSelectUnlock
// Purpose:	unlock given level/stage
// Output:	unlocked
// Return:	none
/////////////////////////////////////
PUBLIC void StageSelectUnlock(hCFG playerProfile, int stage, int level);

/////////////////////////////////////
// Name:	StageSelectAllLevelDone
// Purpose:	check to see if all the
//			levels in a stage are
//			done.
// Output:	none
// Return:	true if all levels are done
/////////////////////////////////////
PUBLIC bool StageSelectAllLevelDone(int stage);

/////////////////////////////////////
// Name:	StageSelectLoadData
// Purpose:	load the level data
// Output:	dat filled
// Return:	true=data exists
/////////////////////////////////////
PUBLIC bool StageSelectLoadData(hCFG playerProfile, int stage, int level, 
								StageDat & dat);

/////////////////////////////////////
// Name:	StageSelectSaveData
// Purpose:	save the level data
//			from world
// Output:	profile set
// Return:	none
/////////////////////////////////////
PUBLIC void StageSelectSaveData(hCFG playerProfile, int stage, int level);

#endif