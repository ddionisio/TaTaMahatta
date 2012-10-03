#ifndef _tata_globals_h
#define _tata_globals_h

#include "tata_world.h"

//this includes all globals
extern HWND	  g_hwnd;				//the windows handle

extern bool   g_bUseVertexLighting;	//are we using light map or vertex lighting?

extern Flag	  g_gameFlag;	//global game flag

extern World *g_world;		//the world interface used by singleplayer
							//this is initialized in 'Game' state

extern double g_hurtInvulDelay;	//delay for invulnerability when hurt

extern char *g_tataTypeName[TATA_MAX]; //tata_tool
extern char *g_enemyTypeName[ENEMY_MAX]; //tata_tool
extern char *g_itmStr[ITM_MAX]; //tata_tool
extern char *g_rcStr[RC_MAX]; //tata_tool
extern char *g_wayptModeStr[WAYPT_MAX]; //tata_tool

extern hBKFX g_bkfx;			//use for special background transition, tata_tool

extern hTXT	 g_shadowTxt;		//the shadow texture, tata_tool

extern string g_MapLoadPath;	//when loading map from console or exec. param
extern string g_cloudMapPath;	//the cloud map filename, tata_tool

extern string g_creditsMapPath;		//the credits map filename, tata_tool
extern string g_creditsScenePath;	//the credits scene filename, tata_tool

extern string g_levelListPath;	//the level list filename, tata_tool

extern hFNT		 g_fntDbg;

extern string	 g_strDbg;		//debug string

extern int		 g_stage, g_level;	//stage and level used in 'level select'

extern float	 g_runScale;		//scale for running

extern const char *g_resName[MAXRES];
extern gfxDisplayMode g_resDat[MAXRES];

#endif