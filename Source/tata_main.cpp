#include "tata_main.h"

#include "tata_basic.h"

#include "resource.h"

//#define FRAME_LIMIT
#define FRAME_LIMIT_DELAY	60

#ifdef FRAME_LIMIT
win32Time g_fLimTimer;
#endif

bool	g_bShowFPS=false;

bool   g_bUseVertexLighting = false;	//are we using light map?

Flag	  g_gameFlag;	//global game flag

string	 g_strDbg;		//debug string

int		 g_gameFps=0;

HWND	  g_hwnd=0;				//the windows handle

hFNT	  g_fntDbg=0;

int		 g_stage=0, g_level=0;	//stage and level used in 'level select'

float	 g_runScale = 0;

///////////////////////////////////////
//Temporary Setup
BOOL CALLBACK TaTaSetupDlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

//This includes the main window
//Game Initialization/Destroy
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,
        int ishow)
{
    HWND hwnd;
    MSG msg;
	int i=0;

	//initialize everything here
	srand(time(0));

	//initialize our save directory for the user
	TataCreateSaveDir();

	char gamePath[DMAXCHARBUFF]={0};
	TataGetGameCfgPath(gamePath);

	//parse the command line
	ParserReadCommandLine(cmdline, '-', CmdLineCallback, 0);

	///////////////////////////////////////
	//Setup
	if(g_gameFlag.CheckFlag(GAME_FLAG_SETUP))
		DialogBox(hinstance, MAKEINTRESOURCE(IDD_TATASETUP), 0, TaTaSetupDlgProc);
	else
	{
		//run setup if game config does not exists
		FILE *fp = fopen(gamePath, "rt");

		if(!fp)
		{
			MessageBox (NULL, "Looks like you're gonna have to set the game up before playing...Don't worry, this won't take long", "Ta-Ta Mahatta Setup", MB_OK);

			TataGenerateGameCfg(gamePath);

			DialogBox(hinstance, MAKEINTRESOURCE(IDD_TATASETUP), 0, TaTaSetupDlgProc);
		}
		else
			fclose(fp);
	}

	if(g_gameFlag.CheckFlag(GAME_FLAG_QUIT))
		goto DEATH;

	//initialize engine with config
	if(TataInit(gamePath, &hwnd, hinstance) != RETCODE_SUCCESS)
		goto DEATH;

	GFXClear(0, 0, 0);
	GFXUpdate(0, 0, 0);
	GFXClear(0, 0, 0);
	GFXUpdate(0, 0, 0);
	//

	GFXHideCursor();

	//initialize script
	TataDisplayLoadText("Initializing Script Engine...");
	if(ScriptTeaseInit() != RETCODE_SUCCESS)
		goto DEATH;

	TataDisplayLoadText("Initializing Script Functions...");
	TataScriptQuery();

	//initialize the data list
	{
		TataDisplayLoadText("Initializing Data File Stuff...");

		char dataFolder[MAXCHARBUFF] = GAMEFOLDER;
		strcat(dataFolder, "\\Data");
		TataDataFillList(dataFolder, "*.ini");
	}

	//initialize global timer
	TimeUpdate();

	//some debug test
	
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	#ifdef FRAME_LIMIT
	//win32Time g_fLimTimer;
	TimeInit(&g_fLimTimer, FRAME_LIMIT_DELAY);
	#endif

	//set to start proc
	if(g_gameFlag.CheckFlag(GAME_FLAG_SKIPINTRO))
		ProcChangeState(PROC_GAME);
	else
		ProcChangeState(PROC_INTRO);
	
    while((msg = GetWindowMessage()).message != WM_QUIT)
	{
		//check to see if device lost
		GFXCheckDeviceLost();

		if(g_gameFlag.CheckFlag(GAME_FLAG_QUIT))
			break;

#ifdef FRAME_LIMIT
		if(TimeElapse(&g_fLimTimer))
		{
#endif
			TimerFPSBegin();

			TimeUpdate();

			//update input
			INPXKeyboardUpdate();
			
			InputUpdate();

			if(INPXKbIsReleased(DIK_F12))
			{
				g_bShowFPS = !g_bShowFPS;
			}

			//update game
			ProcUpdate();

			//GFXClear(0, 0, 0);   //clears screen to given color
			GFXClearDepth();

			GFXBegin();

			//display game
			ProcDisplay();

			//BKFX
			TataUpdateBKFX();

			if(g_bShowFPS)
			{
#ifdef _DEBUG
				char buff[256]={0};
				//sprintf(buff, "num. culled: %d", g_numCulled);
				sprintf(buff, "num. face: %d", g_numFaceProcessed);
				g_strDbg=buff;
#else
				//display debug
				char buff[256]={0};
				sprintf(buff, "fps: %d", g_gameFps);

				g_strDbg=buff;
#endif
				FontPrintf2D(g_fntDbg, 0,0, 0xFFFFFFFF, g_strDbg.c_str());
			}

			GFXEnd();
			
			GFXUpdate(0, 0, 0);  //update frame on display

			g_gameFps = TimerFPSEnd();

#ifdef FRAME_LIMIT
		}
#endif
	}

	//destroy all here
DEATH:

	GFXShowCursor();

	if(g_fntDbg)
		FontDestroy(&g_fntDbg);

	TataSetLoadImage(0);
	TataSetBKFX(0);

	ProcDestroy();

	TataDestroy();

	ScriptTeaseDestroy();

	ErrorDump("error.txt");

	return(msg.wParam);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam)
{
    switch (message)
    {
        
        case WM_CREATE:
			return 0;

		case WM_SIZE:
			switch(wparam)
			{
			case SIZE_MAXIMIZED:
				{
					int ugh = 0;
				}
				break;
			}
			break;

		case WM_MOVE:
            // Our window position has changed, so
            // get the client (drawing) rectangle.
            GetClientRect( hwnd, &G_clientarea);
            // Convert the coordinates from client relative
            // to screen
            ClientToScreen( hwnd, ( LPPOINT )&G_clientarea);
            ClientToScreen( hwnd, ( LPPOINT )&G_clientarea + 1 );

			GFXUpdateClientRect(&G_clientarea);
            return 0;
			
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }

    
    return(DefWindowProc(hwnd, message, wparam, lparam));
}

///////////////////////////////////////
//Temporary Setup
const char *g_resName[MAXRES]={"320x240x16", "320x240x32",
"640x480x16", "640x480x32", "800x600x16", "800x600x32",
"1024x768x16", "1024x768x32"};

gfxDisplayMode g_resDat[MAXRES] = {{320,200,BPP_16,0}, {320,200,BPP_32,0}, 
{640,480,BPP_16,0}, {640,480,BPP_32,0}, {800,600,BPP_16,0}, {800,600,BPP_32,0}, 
{1024,768,BPP_16,0}, {1024,768,BPP_32,0}};

BOOL CALLBACK TaTaSetupDlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static hCFG gameCfg = 0;

	switch(message)
	{
	case WM_INITDIALOG:		//if we're starting up...
		{
			char gamePath[DMAXCHARBUFF]={0};
			TataGetGameCfgPath(gamePath);

			//check for setup existance
			FILE *fp = fopen(gamePath, "rt");

			if(!fp)
			{
				TataGenerateGameCfg(gamePath);
			}
			else
				fclose(fp);

			int index, iDat;

			for(int i = 0; i < MAXRES; i++)
			{
				index = SendMessage(GetDlgItem(hwnd, IDC_RESOLUTION), LB_ADDSTRING, 0, (LONG)(LPSTR)g_resName[i]);
				SendMessage(GetDlgItem(hwnd, IDC_RESOLUTION), LB_SETITEMDATA, index, ( LPARAM )&g_resDat[i]);
			}

			gameCfg = CfgFileLoad(gamePath);

			if(gameCfg)
			{
				unsigned int width;
				eBPP bpp;

				iDat = CfgGetItemInt(gameCfg, "graphics", "width");
				width = iDat == -1 ? 640 : iDat;

				iDat = CfgGetItemInt(gameCfg, "graphics", "bpp");
				bpp = iDat == -1 ? BPP_16 : (eBPP)iDat;

				for(int i = 0; i < MAXRES; i++)
				{
					if(g_resDat[i].width == width
						&& g_resDat[i].bpp == bpp)
						SendMessage(GetDlgItem(hwnd, IDC_RESOLUTION), LB_SELECTSTRING, 1, (LONG)(LPSTR)g_resName[i]);
				}

				iDat = CfgGetItemInt(gameCfg, "graphics", "fullscreen");

				if(iDat == 1)
					CheckDlgButton(hwnd, IDC_FULLSCREEN, BST_CHECKED);
				else
					CheckDlgButton(hwnd, IDC_FULLSCREEN, BST_UNCHECKED);
			}

			ShowWindow(hwnd,SW_SHOW);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case ID_PLAYNSAVE:
			{
				int index = SendMessage(GetDlgItem(hwnd, IDC_RESOLUTION), LB_GETCURSEL, 0, 0);

				gfxDisplayMode *pResDat = (gfxDisplayMode *)SendMessage(GetDlgItem(hwnd, IDC_RESOLUTION), LB_GETITEMDATA, index, 0);

				if(gameCfg)
				{
					CfgAddItemInt(gameCfg, "graphics", "width", pResDat->width);
					CfgAddItemInt(gameCfg, "graphics", "height", pResDat->height);
					CfgAddItemInt(gameCfg, "graphics", "bpp", pResDat->bpp);
					CfgAddItemInt(gameCfg, "graphics", "refreshrate", pResDat->refresh);

					CfgAddItemInt(gameCfg, "graphics", "fullscreen", 
						IsDlgButtonChecked(hwnd, IDC_FULLSCREEN) == BST_CHECKED ? 1 : 0);

					CfgFileSave(gameCfg);

					CfgFileDestroy(&gameCfg); gameCfg = 0;
				}

				EndDialog(hwnd, TRUE);
				return TRUE;
			}

		case ID_PLAYNOSAVE:
			if(gameCfg)
			{ CfgFileDestroy(&gameCfg); gameCfg = 0; }

			EndDialog(hwnd, TRUE);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		{
			g_gameFlag.SetFlag(GAME_FLAG_QUIT, true);
			EndDialog(hwnd, TRUE);
			return TRUE;
		}
		break;
	}

	return FALSE;
}