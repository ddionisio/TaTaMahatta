#include "tata_main.h"

#include "tata_globals.h"

#include <shlobj.h>
#include <direct.h>

//tools of the trade

typedef struct _dataInfo {
	string		name;	//name of data
	string		filePath; //path of data
} dataInfo;

vector<dataInfo> g_dataList; //all data

/*ENTITY_TYPE_PROJECTILE,
	ENTITY_TYPE_ITEM,
	ENTITY_TYPE_OBJECT,
	ENTITY_TYPE_TATA,
	ENTITY_TYPE_ENEMY,

  ENTITY_TYPE_MAXTYPE
*/

char *g_tataTypeName[TATA_MAX] = {"CaptainTaTa", "TaTaTug", "TinkerTa", 
"TaTaTrample", "HopTaHop", "FrostTa", "ParaTaTa", "LoopTaLoop",
"KeyKeyTa", "ChiTa", "TaVatar", "BabyTaTa"};

char *g_enemyTypeName[ENEMY_MAX] = {"FungaBark", "ShroomGuard", 
"ShroomShooter", "FungaSpy", "FungaSmug", "FungaMusketeer", "ShroomPa",
"CatterShroom", "CorrupTa"};

char *g_itmStr[ITM_MAX] = {"resource", "egg", "collection"};

char *g_rcStr[RC_MAX]={"love", "care"};

char *g_wayptModeStr[WAYPT_MAX] = {"stopatend", "repeat", "backtrace"};

string g_cloudMapPath;
string g_MapLoadPath;	//when loading map from console or exec. param

string g_creditsMapPath;
string g_creditsScenePath;

string g_levelListPath;

hBKFX g_bkfx=0;			//use for special background transition

hTXT g_loadImg=0;		//the load image

hTXT	 g_shadowTxt=0;		//the shadow texture, tata_tool

/////////////////////////////////////
// Name:	TataGetType
// Purpose:	get Ta-Ta type from name
// Output:	none
// Return:	the Ta-Ta type, -1 means 
//			unknown Ta-Ta
/////////////////////////////////////
PUBLIC int TataGetType(const char *name)
{
	for(int i = 0; i < TATA_MAX; i++)
	{
		if(stricmp(g_tataTypeName[i], name) == 0)
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	EnemyGetType
// Purpose:	get Enemy type from name
// Output:	none
// Return:	the Enemy type, -1 means 
//			unknown Enemy
/////////////////////////////////////
PUBLIC int EnemyGetType(const char *name)
{
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		if(stricmp(g_enemyTypeName[i], name) == 0)
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	ItemGetType
// Purpose:	get Item type from name
// Output:	none
// Return:	the item type
/////////////////////////////////////
PUBLIC int ItemGetType(const char *name)
{
	for(int i = 0; i < ITM_MAX; i++)
	{
		if(stricmp(g_itmStr[i], name) == 0)
			return i;
	}

	return -1;
}

/////////////////////////////////////
// Name:	WaypointGetMode
// Purpose:	get the waypoint mode type
//			from name.
// Output:	none
// Return:	the waypoint mode type, 
//			WAYPT_STOPATEND for unknown.
/////////////////////////////////////
PUBLIC eWayPtMode WaypointGetMode(const char *str)
{
	for(int i = 0; i < WAYPT_MAX; i++)
	{
		if(stricmp(str, g_wayptModeStr[i]) == 0)
			return (eWayPtMode)i;
	}

	return WAYPT_STOPATEND;
}

/////////////////////////////////////
// Name:	TataDataGetFilePath
// Purpose:	get the filepath of given
//			data name
// Output:	none
// Return:	file path, 0 if not found
/////////////////////////////////////
PUBLIC const char *TataDataGetFilePath(const char *name)
{
	for(int i = 0; i < g_dataList.size(); i++)
	{
		if(stricmp(g_dataList[i].name.c_str(), name) == 0)
			return g_dataList[i].filePath.c_str();
	}

	return 0;
}

PROTECTED void DataDirCallback(char *filepath, void *userData)
{
	char name[MAXCHARBUFF]={0};

	GetFilename(filepath, name, MAXCHARBUFF-1);

	//make sure this data does not exist
	if(TataDataGetFilePath(name) == 0)
	{
		dataInfo newDataInfo;

		newDataInfo.name = name;
		newDataInfo.filePath = filepath;

		g_dataList.push_back(newDataInfo);
	}
}

/////////////////////////////////////
// Name:	TataDataFillList
// Purpose:	fill in the data
// Output:	data list updated
// Return:	none
/////////////////////////////////////
PUBLIC void TataDataFillList(char *dirPath, char *pattern)
{
	ParserFindFiles(dirPath, pattern, true, DataDirCallback, 0);
}

/////////////////////////////////////
// Name:	TataSetBKFX
// Purpose:	set a new bkfx
// Output:	bkfx added
// Return:	none
/////////////////////////////////////
PUBLIC void TataSetBKFX(hBKFX bkfx)
{
	if(g_bkfx)
		BKFXDestroy(&g_bkfx);

	g_bkfx = bkfx;
}

/////////////////////////////////////
// Name:	TataUpdateBKFX
// Purpose:	update and display bkfx
// Output:	bkfx super special fx
// Return:	none
/////////////////////////////////////
PUBLIC void TataUpdateBKFX()
{
	if(g_bkfx)
	{
		//GFXBegin();

		if(BKFXUpdate(g_bkfx) == RETCODE_BREAK)
		{ BKFXDestroy(&g_bkfx); g_bkfx = 0; }
		else
			BKFXDisplay(g_bkfx);

		//GFXEnd();
	}
}

/////////////////////////////////////
// Name:	TataSetLoadImage
// Purpose:	set the load image
// Output:	load image set
// Return:	none
/////////////////////////////////////
PUBLIC void TataSetLoadImage(hTXT txt)
{
	if(g_loadImg)
		TextureDestroy(&g_loadImg);

	g_loadImg = txt;
}

/////////////////////////////////////
// Name:	TataDisplayLoad()
// Purpose:	display load
// Output:	screen update
// Return:	none
/////////////////////////////////////
PUBLIC void TataDisplayLoad()
{
	if(g_loadImg)
	{
		int x = (GFXGetScrnSize().cx/2)-(TextureGetWidth(g_loadImg)/2), 
			y = (GFXGetScrnSize().cy/2)-(TextureGetHeight(g_loadImg)/2);

		DWORD clr = D3DCOLOR_RGBA(255,255,255,255);

		GFXBegin();
		GFXBltModeEnable(0);
		
		TextureBlt(g_loadImg, x, y, 0, &clr, 0);

		GFXBltModeDisable();
		GFXEnd();

		GFXUpdate(0, 0, 0);  //update frame on display

		GFXBegin();
		GFXBltModeEnable(0);
		
		TextureBlt(g_loadImg, x, y, 0, &clr, 0);

		GFXBltModeDisable();
		GFXEnd();

		GFXUpdate(0, 0, 0);  //update frame on display
	}
}

/////////////////////////////////////
// Name:	TataDisplayLoadText
// Purpose:	display a text message
//			instantly at the bottom
// Output:	screen update
// Return:	none
/////////////////////////////////////
PUBLIC void TataDisplayLoadText(const char *str, ...)
{
	//g_fntDbg
	if(g_fntDbg)
	{
		char buff[DMAXCHARBUFF];								// Holds Our String

		//do the printf thing
		va_list valist;
		va_start(valist, str);
		_vsnprintf(buff, sizeof(buff), str, valist);
		va_end(valist);

		SIZE fntSize; FontGetStrSize(g_fntDbg, (char*)str, &fntSize);
		D3DRECT area;
		area.y1 = GFXGetScrnSize().cy-fntSize.cy;
		area.x1 = 0;
		area.x2 = GFXGetScrnSize().cx;
		area.y2 = GFXGetScrnSize().cy;

		GFXClear(1, &area, 0);

		GFXBegin();

		FontPrintf2D(g_fntDbg, 0, area.y1, 0xffffffff, buff);

		GFXEnd();

		GFXUpdate(0, 0, 0);  //update frame on display
	}
}

/////////////////////////////////////
// Name:	TataSaveGameCfg
// Purpose:	save the current config to
//			file
// Output:	config save
// Return:	none
/////////////////////////////////////
PUBLIC void TataSaveGameCfg()
{
	char gamePath[DMAXCHARBUFF]={0};
	TataGetGameCfgPath(gamePath);

	//load the config file
	hCFG cfg = CfgFileLoad(gamePath);

	if(cfg)
	{
		//save resolution
		gfxDisplayMode mode;
		GFXGetMode(&mode);

		CfgAddItemInt(cfg, "graphics", "width", (int)mode.width);
		CfgAddItemInt(cfg, "graphics", "height", (int)mode.height);
		CfgAddItemInt(cfg, "graphics", "bpp", (int)mode.bpp);
		CfgAddItemInt(cfg, "graphics", "refreshrate", (int)mode.refresh);

		//save lighting
		CfgAddItemInt(cfg, "graphics", "lightmap", (int)(!g_bUseVertexLighting));

		//save sound stuff
		//set up audio's music/sound volume
		DWORD sVol, stVol, mVol;

		BASS_GetGlobalVolumes(&mVol, &sVol, &stVol);

		CfgAddItemInt(cfg, "audio", "soundvol", sVol);
		CfgAddItemInt(cfg, "audio", "musicvol", stVol);

		//save input stuff
		InputSave(cfg);

		CfgFileSave(cfg);
		
		CfgFileDestroy(&cfg);
	}
}

PUBLIC void TataShadowDestroy()
{
	if(g_shadowTxt)
	{
		TextureDestroy(&g_shadowTxt);
		g_shadowTxt = 0;
	}
}

PUBLIC void TataShadowInit()
{
	TataShadowDestroy();
	g_shadowTxt = TextureCreateAlphaCircle("shadow", 0, 0, 0, 0, 64);
}

PUBLIC float TataRand(float a, float b)
{
	int r;
    float	x;
    
	r = rand ();
    x = (float)(r & 0x7fff) /
		(float)0x7fff;
    return (x * (b - a) + a);
}

//parser callback for commandline
void CmdLineCallback(const char *cmdline, void *userData)
{
	char *pBuff=(char*)cmdline;
	char cmdStr[MAXCHARBUFF], paramStr[MAXCHARBUFF];

	if(cmdline)
	{
		ParserReadWordBuff(&pBuff, cmdStr, MAXCHARBUFF, ' ');
		ParserReadWordBuff(&pBuff, paramStr, MAXCHARBUFF, ' ');

		if(stricmp(cmdStr, "setup") == 0)
			g_gameFlag.SetFlag(GAME_FLAG_SETUP, true);
		else if(stricmp(cmdStr, "map") == 0)
		{
			g_MapLoadPath = GAMEFOLDER;
			g_MapLoadPath += "\\Maps\\";
			g_MapLoadPath += paramStr;

			g_gameFlag.SetFlag(GAME_FLAG_MAP, true);
			g_gameFlag.SetFlag(GAME_FLAG_SKIPINTRO, true);
		}
		else if(stricmp(cmdStr, "skipintro") == 0)
			g_gameFlag.SetFlag(GAME_FLAG_SKIPINTRO, true);
	}
}

//This is used for all particles that collide with the world
bool ParticleCollisionCallback(float pt1[eMaxPt], float pt2[eMaxPt], float *tOut, float nOut[eMaxPt])
{
	if(g_world && g_world->GetMap())
	{
		gfxTrace trace;

		QBSPCollision(g_world->GetMap(), 0, 0, pt1, pt2, &trace, 1, -1);

		*tOut = trace.t;

		memcpy(nOut, trace.norm, sizeof(trace.norm));

		return trace.t < 1 ? true : false;
	}

	return false;
}

//construct the save game directory in My Documents folder
void TataCreateSaveDir()
{
	string savePath;

	char winPath[MAX_PATH]={0};
	if(SHGetSpecialFolderPath(NULL, winPath, CSIDL_PERSONAL, TRUE))
	{
		savePath = winPath;
		savePath += "\\";
		savePath += MSAVEFOLDER;
		_mkdir(savePath.c_str());

		savePath += "\\";
		savePath += SAVEFOLDER;
		_mkdir(savePath.c_str());
	}
}

//get the game.cfg filepath
void TataGetGameCfgPath(char *strOut)
{
	char winPath[MAX_PATH]={0};
	if(SHGetSpecialFolderPath(NULL, winPath, CSIDL_PERSONAL, TRUE))
	{
		string savePath(winPath);
		savePath += "\\";
		savePath += MSAVEFOLDER;

		savePath += "\\game.cfg";
		
		strcpy(strOut, savePath.c_str());
	}
	else
		strcpy(strOut, GAMECFGFILE);
}

//get the save game directory
void TataGetSaveDir(char *strOut)
{
	string savePath;

	char winPath[MAX_PATH]={0};
	if(SHGetSpecialFolderPath(NULL, winPath, CSIDL_PERSONAL, TRUE))
	{
		savePath = winPath;
		savePath += "\\";
		savePath += MSAVEFOLDER;

		savePath += "\\";
		savePath += SAVEFOLDER;
	}
	else
	{
		savePath = GAMEFOLDER;
		savePath += "\\";
		savePath += SAVEFOLDER;
	}

	strcpy(strOut, savePath.c_str());
}