#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_stage_select.h"

#include "tata_globals.h"

//FIXME: there should only be one instance of Level Select
//		 make sure that we cannot create this menu more than once

#define LEVEL_SELECT_SECTION	"level_select"

#define STAGE_FADE_DELAY	1000

#define NUM_CURSOR_LAYER	3

hTXT g_cursorTxt[NUM_CURSOR_LAYER]={0};

hTXT g_dotTxt[ST_MAX]={0};

Move g_cursorRotSpd1, g_cursorRotSpd2;

win32Time g_cursorMoveDelay;

///////////////////////////////////////////////
//all locations and size are in terms of screen
D3DXVECTOR2 g_cursorSize;	

D3DXVECTOR2 g_stageLoc;
D3DXVECTOR2 g_stageSize;

D3DXVECTOR2 g_dotSize;

///////////////////////////////////////////////
//Update gooks
win32Time g_fadeDelay;

int g_prevLevel;

float g_tFadeDelay, g_tSelDelay, g_rot1, g_rot2;

D3DXVECTOR2 g_curSelLoc;

DWORD g_lvlColor;	//used by stage draw, cursor

RETCODE MCB_LevelSelect(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		{
			SIZE scrnSize = GFXGetScrnSize();

			hCFG cfg = (hCFG)wParam;

			if(cfg)
			{
				string imgPath;
				char buff[MAXCHARBUFF]={0}, path[MAXCHARBUFF]={0};

				int i;

				/////////////////////////////////////////////////////////////
				//Load the cursor images
				for(i = 0; i < NUM_CURSOR_LAYER; i++)
				{
					sprintf(buff, "cursor_%d_img", i+1);

					if(CfgGetItemStr(cfg, LEVEL_SELECT_SECTION, buff, path))
					{
						imgPath = GAMEFOLDER;
						imgPath += "\\";
						imgPath += path;

						g_cursorTxt[i] = TextureCreate(0, imgPath.c_str(), false, 0);
					}
				}

				/////////////////////////////////////////////////////////////
				//load cursor components
				g_cursorRotSpd1.MoveSetSpeed(CfgGetItemFloat(cfg, LEVEL_SELECT_SECTION, "cursor_rot_spd_1"));
				g_cursorRotSpd2.MoveSetSpeed(CfgGetItemFloat(cfg, LEVEL_SELECT_SECTION, "cursor_rot_spd_2"));

				TimeInit(&g_cursorMoveDelay, CfgGetItemFloat(cfg, LEVEL_SELECT_SECTION, "cursor_move_delay"));

				if(CfgGetItemStr(cfg, LEVEL_SELECT_SECTION, "cursor_size", buff))
				{
					sscanf(buff, "%f,%f", &g_cursorSize.x, &g_cursorSize.y);

					g_cursorSize.x *= scrnSize.cx;
					g_cursorSize.y *= scrnSize.cy;
				}

				/////////////////////////////////////////////////////////////
				//load stage stuff components
				if(CfgGetItemStr(cfg, LEVEL_SELECT_SECTION, "stage_sLoc", buff))
				{
					sscanf(buff, "%f,%f", &g_stageLoc.x, &g_stageLoc.y);

					g_stageLoc.x *= scrnSize.cx;
					g_stageLoc.y *= scrnSize.cy;
				}

				if(CfgGetItemStr(cfg, LEVEL_SELECT_SECTION, "stage_eLoc", buff))
				{
					D3DXVECTOR2 sELoc;
					sscanf(buff, "%f,%f", &sELoc.x, &sELoc.y);

					sELoc.x *= scrnSize.cx; g_stageSize.x = sELoc.x-g_stageLoc.x;
					sELoc.y *= scrnSize.cy; g_stageSize.y = sELoc.y-g_stageLoc.y;
				}

				/////////////////////////////////////////////////////////////
				//load dot components 
				for(i = 0; i < ST_MAX; i++)
				{
					sprintf(buff, "dot_%d_img", i);

					if(CfgGetItemStr(cfg, LEVEL_SELECT_SECTION, buff, path))
					{
						imgPath = GAMEFOLDER;
						imgPath += "\\";
						imgPath += path;

						g_dotTxt[i] = TextureCreate(0, imgPath.c_str(), false, 0);
					}
				}

				if(CfgGetItemStr(cfg, LEVEL_SELECT_SECTION, "level_dot_size", buff))
				{
					sscanf(buff, "%f,%f", &g_dotSize.x, &g_dotSize.y);

					g_dotSize.x *= scrnSize.cx;
					g_dotSize.y *= scrnSize.cy;
				}
			}

			//disable input
			g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);

			/////////////////////////////////////////////////////////////
			//set the update variable stuff
			TimeInit(&g_fadeDelay, STAGE_FADE_DELAY);
			
			g_prevLevel = g_level;

			g_tFadeDelay = 0;

			g_tSelDelay = 1;

			g_rot1 = g_rot2 = 0;

			g_curSelLoc = StageSelectGetLevelLoc(g_stage, g_level);

			g_curSelLoc.x -= g_cursorSize.x/2;
			g_curSelLoc.y -= g_cursorSize.y/2;

			g_lvlColor = 0;

			if(g_level >= StageSelectGetNumLevels(g_stage))
				g_level = 0;
		}
		break;

	case MENU_MSG_BTN:
		/////////////////////////////////////////////////////////////
		//update controls
		if(lParam == INP_STATE_RELEASED)
		{
			int l=g_level;
			switch(wParam)
			{
			case INP_LEFT:
				if(g_tSelDelay >= 1)
				{
					while(l > 0)
					{
						l--;

						if(StageSelectGetLevelState(g_stage, l) != ST_LOCK)
						{
							g_prevLevel = g_level;
							g_level = l;
							g_tSelDelay = 0;
							TimeReset(&g_cursorMoveDelay);
							break;
						}
					}
				}
				break;

			case INP_RIGHT:
				if(g_tSelDelay >= 1)
				{
					while(l < StageSelectGetNumLevels(g_stage))
					{
						l++;

						if(StageSelectGetLevelState(g_stage, l) != ST_LOCK)
						{
							g_prevLevel = g_level;
							g_level = l;
							g_tSelDelay = 0;
							TimeReset(&g_cursorMoveDelay);
							break;
						}
					}
				}
				break;
			}
		}
		break;

	case MENU_MSG_ITEM:
		switch(wParam)
		{
		case LEVELSELECT_OK:
			{
				//re-enable input
				if(g_world)
					g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

				//set the 'prev' as level select map.
				g_world->MapSetPrev();

				//get map file, load it, exit all menus
				const char *lvlPath = StageSelectGetLevelMap(g_stage, g_level);

				if(lvlPath)
				{
					if(g_world->MapLoad(lvlPath) == RETCODE_SUCCESS)
					{
						//set the level name
						g_world->SetName(StageSelectGetLevelName(g_stage, g_level));

						//set the stage and level number
						g_world->SetStageLevel(g_stage, g_level);

						MenuExitAll();

						//load in-game menu
						MenuGameAdd(GAMEMENU_PAUSEINGAME);

						//load the level start description
						if(g_world->MapGetDesc() != 0 
							&& g_world->MapGetDesc()[0] != 0)
						{
							MenuGameAdd(GAMEMENU_LEVELSTART);
							MenuEnableCurrent(true);
						}

						//super special bkfx
						fadeinout_init sBKFX;
						sBKFX.r = sBKFX.g = sBKFX.b = 255;
						sBKFX.delay = 2000;
						TataSetBKFX(BKFXCreate(eBKFX_FADEINOUT, &sBKFX));
					}
				}
			}
			break;

		case LEVELSELECT_CANCEL:
			//re-enable input
			if(g_world)
				g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

			g_level = 0;

			//get out
			MenuExitCurrent();
			break;
		}
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			{
				/////////////////////////////////////////////////////////////
				//set level name description
				hMenu->SendItemMessage(LEVELSELECT_LVLTXT, MENU_ITM_MSG_SETTEXT, 
								(WPARAM)StageSelectGetLevelName(g_stage, g_level), 0);

				/////////////////////////////////////////////////////////////
				//set records
				if(g_world && g_world->ProfileGetCfg())
				{
					StageDat dat;
					bool bLoaded = StageSelectLoadData(g_world->ProfileGetCfg(), 
						g_stage, g_level, dat);

					char buff[MAXCHARBUFF]={0};

					if(bLoaded)
						sprintf(buff, "BEST TIME: %02d:%02d:%02d", dat.hr, dat.min, dat.sec);
					else
						sprintf(buff, "BEST TIME: --:--:--");

					hMenu->SendItemMessage(LEVELSELECT_BESTTIME, MENU_ITM_MSG_SETTEXT, 
								(WPARAM)buff, 0);

					if(dat.artafactMax > 0)
						sprintf(buff, "AR-TA-FACT: %d/%d", dat.artafactMin, dat.artafactMax);
					else if(!bLoaded)
						sprintf(buff, "AR-TA-FACT: ????");
					else
						sprintf(buff, "AR-TA-FACT: N/A");

					hMenu->SendItemMessage(LEVELSELECT_ARTAFACTS, MENU_ITM_MSG_SETTEXT, 
								(WPARAM)buff, 0);
				}

				/////////////////////////////////////////////////////////////
				//update selection mov't
				if(g_tSelDelay < 1)
				{
					g_tSelDelay = TimeGetTime(&g_cursorMoveDelay)/TimeGetDelay(&g_cursorMoveDelay);

					if(g_tSelDelay > 1)
					{
						g_tSelDelay = 1;

						TimeReset(&g_cursorMoveDelay);
					}

					D3DXVec2Lerp(&g_curSelLoc,
						&StageSelectGetLevelLoc(g_stage, g_prevLevel),
						&StageSelectGetLevelLoc(g_stage, g_level),
						g_tSelDelay);

					g_curSelLoc.x -= g_cursorSize.x/2;
					g_curSelLoc.y -= g_cursorSize.y/2;
				}

				/////////////////////////////////////////////////////////////
				//update rotate
				g_rot1 += g_cursorRotSpd1.MoveUpdate(g_timeElapse);
				g_rot2 += g_cursorRotSpd2.MoveUpdate(g_timeElapse);
			}
			break;

		case MENU_UPDATE_ENTERING:
			/////////////////////////////////////////////////////////////
			//update fade
			if(g_tFadeDelay < 1)
			{
				g_tFadeDelay = TimeGetTime(&g_fadeDelay)/TimeGetDelay(&g_fadeDelay);

				if(g_tFadeDelay > 1)
				{
					g_tFadeDelay = 1;

					TimeReset(&g_fadeDelay);
				}

				//set color
				g_lvlColor = D3DCOLOR_RGBA(255,255,255,(BYTE)(g_tFadeDelay*255));
			}
			else
			{ g_tFadeDelay = 0; return RETCODE_BREAK; }
			break;

		case MENU_UPDATE_EXITING:
			/////////////////////////////////////////////////////////////
			//update fade
			if(g_tFadeDelay < 1)
			{
				g_tFadeDelay = TimeGetTime(&g_fadeDelay)/TimeGetDelay(&g_fadeDelay);

				if(g_tFadeDelay > 1)
				{
					g_tFadeDelay = 1;

					TimeReset(&g_fadeDelay);
				}

				//set color
				g_lvlColor = D3DCOLOR_RGBA(255,255,255,(BYTE)(255 - g_tFadeDelay*255));
			}
			else
			{ return RETCODE_BREAK; }
			break;
		}
		break;

	case MENU_MSG_DRAW:
		{
			GFXBltModeEnable(0);
				
			StageSelectDisplay(g_stage, g_stageLoc, g_stageSize, 
					g_dotTxt, g_dotSize.x, g_lvlColor);

			/////////////////////////////////////////////////////////////
			//display the cursor
			if(g_cursorTxt[0])
				TextureStretchBlt(g_cursorTxt[0], g_curSelLoc.x, g_curSelLoc.y, 
					g_cursorSize.x, g_cursorSize.y, 0, &g_lvlColor, g_rot1);

			if(g_cursorTxt[1])
				TextureStretchBlt(g_cursorTxt[1], g_curSelLoc.x, g_curSelLoc.y, 
					g_cursorSize.x, g_cursorSize.y, 0, &g_lvlColor, g_rot2);

			if(g_cursorTxt[2])
				TextureStretchBlt(g_cursorTxt[2], g_curSelLoc.x, g_curSelLoc.y, 
					g_cursorSize.x, g_cursorSize.y, 0, &g_lvlColor, 0);

			GFXBltModeDisable();
		}
		break;

	case MENU_MSG_DESTROY:
		{
			int i;

			for(i = 0; i < NUM_CURSOR_LAYER; i++)
			{
				if(g_cursorTxt[i])
				{ TextureDestroy(&g_cursorTxt[i]); g_cursorTxt[i] = 0; }
			}

			for(i = 0; i < ST_MAX; i++)
			{
				if(g_dotTxt[i])
				{ TextureDestroy(&g_dotTxt[i]); g_dotTxt[i] = 0; }
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}
