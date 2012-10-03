#include "tata_main.h"

#include "resource.h"

#include "tata_globals.h"

#include "tata_sound.h"

//Load the game config
//Initialize all engine

PUBLIC void TataGenerateGameCfg(const char *configFile)
{
	const char GameCfgStr[] =
		"[graphics]\n"\
		"fullscreen=1\n"\
		"width=640\n"\
		"height=480\n"\
		"bpp=0\n"\
		"refreshrate=0\n"\
		"lightmap=1\n"\
		"[audio]\n"\
		"enable=1\n"\
		"3DAlgo=3DALG_DEFAULT\n"\
		"frequency=44100\n"\
		"musicvol=100\n"\
		"soundvol=100\n"\
		"[game]\n"\
		"cloudLevel=Maps\\cloud.bsp\n"\
		"creditsLevel=Maps\\credits.bsp\n"\
		"creditsScene=Maps\\Credits_The_Makers.scn\n"\
		"tutorialLevel=Maps\\tutorial_basic.bsp\n"\
		"levelList=levels.cfg\n"\
		"soundList=sounds.cfg\n"\
		"loadImg=Textures\\Misc\\loading.tga\n"\
		"viewrotX=-20\n"\
		"viewrotY=0\n"\
		"viewrotZ=0\n"\
		"viewdist=160\n"\
		"viewspd=3\n"\
		"downspd=300\n"\
		"hurtInvulDelay=2000\n"\
		"runScale=2\n"\
		"[HUD]\n"\
		"blank=textures\\portraits\\blank_port.jpg\n"\
		"select=textures\\portraits\\select_port.tga\n"\
		"ouch=textures\\portraits\\ouch_port.tga\n"\
		"health=textures\\portraits\\healthpt.tga\n"\
		"fntSize=0.028125\n"\
		"fntType=Comic Sans\n"\
		"goalSX=0.7875\n"\
		"goalSY=0.808333\n"\
		"goalEX=0.8859375\n"\
		"goalEY=0.939583\n"\
		"goalFNTX=0.89375\n"\
		"goalFNTY=0.833333\n"\
		"startX=0.0234375\n"\
		"startY=0.80833\n"\
		"endX=0.121875\n"\
		"endY=0.939583\n"\
		"space=0.03125\n"\
		"selectMDL=Models\\Misc\\select.ms3d\n"\
		"selectDelay=3000\n"\
		"selectRotSpd=4\n"\
		"[input]\n"\
		"tapDelay=150\n";

	FILE *fp = fopen(configFile, "wt");

	if(fp)
	{
		fprintf(fp, GameCfgStr);
		fclose(fp);
	}
}

/////////////////////////////////////
// Name:	TataInit
// Purpose:	this will load the game
//			config and initialize the game
//			engine
// Output:	stuff initialized
// Return:	success if all goes well
/////////////////////////////////////
PUBLIC RETCODE TataInit(const char *configFile, HWND *hwnd, HINSTANCE hinst)
{
	char filePath[MAXCHARBUFF]={0};
	BOOL bassRet;

	RETCODE ret=RETCODE_SUCCESS;

	//load the config file
	hCFG cfg = CfgFileLoad(configFile);

	if(cfg)
	{
		int iDat, fullscreen;
		int freq, sVol, mVol;

		////////////////////////////////////////////////////
		//load graphics
		gfxDisplayMode gfxMode;

		iDat = CfgGetItemInt(cfg, "graphics", "width");
		gfxMode.width = iDat == -1 ? 640 : iDat;

		iDat = CfgGetItemInt(cfg, "graphics", "height");
		gfxMode.height = iDat == -1 ? 480 : iDat;

		iDat = CfgGetItemInt(cfg, "graphics", "bpp");
		gfxMode.bpp = iDat == -1 ? BPP_16 : (eBPP)iDat;

		iDat = CfgGetItemInt(cfg, "graphics", "refreshrate");
		gfxMode.refresh = iDat == -1 ? 0 : iDat;

		fullscreen = CfgGetItemInt(cfg, "graphics", "fullscreen");

		if(fullscreen == 1)
		{
			*hwnd = Create_Window_Popup(WINNAME, WinProc, hinst, NULL, NULL, 
							    0, 0, 800, 600, IDI_TATAICON);

			ret = GFXInit(*hwnd, &gfxMode);
			//ret = GFXInit(*hwnd, 0);
		}
		else
		{
			*hwnd = Create_Window_Overlapped(WINNAME, 0, 0, WinProc, hinst, NULL, NULL, 
							    0, 0, SCRN_W+8, SCRN_H+27, IDI_TATAICON);

			ret = GFXInit(*hwnd, 0);
		}

		//set global HWND
		g_hwnd = *hwnd;

		if(ret != RETCODE_SUCCESS)
		{ 
			MESSAGE_BOX("Make sure you have DirectX 9 INSTALLED and that your GRAPHICS CARD supports 3D ACCELERATION", "Graphics Initialization Failed"); 
			ErrorMsg("TataInit", "Error Initializing Graphics, makes sure that you installed Direct X 9"); 
			ShellExecute(g_hwnd, "open", "explorer", "http://www.microsoft.com/windows/directx/default.aspx", 0, SW_SHOWNORMAL);
			goto CFGDONE; 
		}

		GFXEnableLight(true);
		GFXSetMaxLight(GAME_LIGHT_MAX);

		g_bUseVertexLighting = CfgGetItemInt(cfg, "graphics", "lightmap") == 1 ? false : true;
		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//load audio
		int isAudio = CfgGetItemInt(cfg, "audio", "enable");

		//set 3d algo
/*		BASS_Set3DAlgorithm(
    DWORD algo
);*/
		char SndAlgo[MAXCHARBUFF];
		if(CfgGetItemStr(cfg, "audio", "3DAlgo", SndAlgo))
		{
			if(stricmp(SndAlgo, "3DALG_DEFAULT") == 0)
				BASS_Set3DAlgorithm(BASS_3DALG_DEFAULT);
			else if(stricmp(SndAlgo, "3DALG_OFF") == 0)
				BASS_Set3DAlgorithm(BASS_3DALG_OFF);
			else if(stricmp(SndAlgo, "3DALG_FULL") == 0)
				BASS_Set3DAlgorithm(BASS_3DALG_FULL);
			else if(stricmp(SndAlgo, "3DALG_LIGHT") == 0)
				BASS_Set3DAlgorithm(BASS_3DALG_LIGHT);
		}

		g_gameFlag.SetFlag(GAME_FLAG_NOSOUND, isAudio == 0 ? true : false);
		
		//
		if(!g_gameFlag.CheckFlag(GAME_FLAG_NOSOUND))
		{
			freq = CfgGetItemInt(cfg, "audio", "frequency"); if(freq == -1) { freq = 22050; }
			sVol = CfgGetItemInt(cfg, "audio", "soundvol");
			mVol = CfgGetItemInt(cfg, "audio", "musicvol");

			/*
			if(BASS_ErrorGetCode() == BASS_ERROR_UNKNOWN)
					assert(0);
			*/

			bassRet = BASS_Init(-1, freq, BASS_DEVICE_3D | BASS_DEVICE_LEAVEVOL, *hwnd);

			if(!bassRet) {

				if(BASS_ErrorGetCode() == BASS_ERROR_NO3D)
					bassRet = BASS_Init(-1, freq, BASS_DEVICE_LEAVEVOL, *hwnd);

				// couldn't initialize device, so use no sound
				if(!bassRet)
					BASS_Init(-2,freq,0,*hwnd);
			}

			//set up audio's music/sound volume
			BASS_SetGlobalVolumes(mVol, sVol, mVol);

			BASS_Set3DFactors(1.0f, 1.0f, 1.0f);

			// Turn EAX off (volume=0.0), if error then EAX is not supported
			BASS_SetEAXParameters(-1,0.0,-1.0,-1.0);

			BASS_Start();

			//BASS_Set3DFactors(1.0f, 1.0f, 1.0f);
			//BASS_Apply3D();
		}
		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//load input

		//Initialize Input
		ret = INPXInit(*hwnd, hinst, INPUTCOOPLVL);

		if(ret != RETCODE_SUCCESS)
		{ ErrorMsg("TataInit", "Error initializing input!!!"); goto CFGDONE; }

		ret = INPXKeyboardInit(*hwnd);

		if(ret != RETCODE_SUCCESS)
		{ ErrorMsg("TataInit", "Error initializing keyboard!!!"); goto CFGDONE; }

		//load the configuration
		InputLoad(cfg);
		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//load cloud level map file name
		if(CfgGetItemStr(cfg, GAME_SECTION, "cloudLevel", filePath))
		{
			g_cloudMapPath = GAMEFOLDER;
			g_cloudMapPath += "\\";
			g_cloudMapPath += filePath;
		}

		////////////////////////////////////////////////////
		//credits stuff
		if(CfgGetItemStr(cfg, GAME_SECTION, "creditsLevel", filePath))
		{
			g_creditsMapPath = GAMEFOLDER;
			g_creditsMapPath += "\\";
			g_creditsMapPath += filePath;
		}

		if(CfgGetItemStr(cfg, GAME_SECTION, "creditsScene", filePath))
		{
			g_creditsScenePath = GAMEFOLDER;
			g_creditsScenePath += "\\";
			g_creditsScenePath += filePath;
		}
		
		//load level list file name
		if(CfgGetItemStr(cfg, GAME_SECTION, "levelList", filePath))
		{
			g_levelListPath = GAMEFOLDER;
			g_levelListPath += "\\";
			g_levelListPath += filePath;
		}

		//load "load" image.
		if(CfgGetItemStr(cfg, GAME_SECTION, "loadImg", filePath))
		{
			string imgPath = GAMEFOLDER;
			imgPath += "\\";
			imgPath += filePath;

			TataSetLoadImage(TextureCreate(0, imgPath.c_str(), false, 0));
		}

		//initialize debug font
		g_fntDbg = FontCreate("Arial", (unsigned int)((12.0/SCRN_W)*GFXGetScrnSize().cx), 0);

		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		//load the sounds
		if(!g_gameFlag.CheckFlag(GAME_FLAG_NOSOUND))
		{
			TataDisplayLoad();

			if(CfgGetItemStr(cfg, GAME_SECTION, "soundList", filePath))
			{
				string sndPath = GAMEFOLDER;
				sndPath += "\\";
				sndPath += filePath;

				GFXClear(0, 0, 0);
				GFXUpdate(0, 0, 0);
				GFXClear(0, 0, 0);
				GFXUpdate(0, 0, 0);

				TataDisplayLoadText("Loading Sounds...");

				TaTaSoundLoad(sndPath.c_str());
			}
		}

		//set run scale
		g_runScale = CfgGetItemFloat(cfg, GAME_SECTION, "runScale");

CFGDONE:
		CfgFileDestroy(&cfg);
	}
	else
	{ ErrorMsg("TataInit", "Unable to load config file!!!"); return RETCODE_FAILURE; }

	//init shadow
	TataShadowInit();

	return ret;
}