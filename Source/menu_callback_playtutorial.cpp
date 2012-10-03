#include "tata_main.h"

#include "tata_menu.h"

#include "tata_menu_game.h"

#include "tata_globals.h"

RETCODE MCB_PlayTutorial(hMENU hMenu, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case MENU_MSG_LOAD:
		g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, true);
		break;

	case MENU_MSG_BTN:
		break;

	case MENU_MSG_ITEM:
		MenuExitCurrent();

		g_world->HUDActive(true);
		g_world->SetFlag(WORLD_FLAG_INPUT_DISABLE, false);

		//which button pressed?
		switch(wParam)
		{
		case PLAYTUTORIAL_YES:
			{
				g_world->SetStageLevel(-1, -1);

				//load in-game menu
				MenuGameAdd(GAMEMENU_PAUSEINGAME);

				g_world->MapSetPrev();

				//load map tutorial
				char gamePath[DMAXCHARBUFF]={0};
				TataGetGameCfgPath(gamePath);
				hCFG cfg = CfgFileLoad(gamePath);

				if(cfg)
				{
					char path[MAXCHARBUFF];
					
					if(CfgGetItemStr(cfg, GAME_SECTION, "tutorialLevel", path))
					{
						string tutorialPath;
						tutorialPath = GAMEFOLDER;
						tutorialPath += "\\";
						tutorialPath += path;

						if(g_world)
							g_world->MapLoad(tutorialPath.c_str());
					}

					CfgFileDestroy(&cfg);
				}
			}
			break;

		case PLAYTUTORIAL_NO:
			//load cloud-in-game menu
			MenuGameAdd(GAMEMENU_PAUSECLOUD);
			break;
		}
		break;

	case MENU_MSG_UPDATE:
		switch(wParam)
		{
		case MENU_UPDATE_NORMAL:
			break;

		case MENU_UPDATE_ENTERING:
		case MENU_UPDATE_EXITING:
			return RETCODE_BREAK;
		}
		break;

	case MENU_MSG_ENABLE:
		break;

	case MENU_MSG_DRAW:
		break;
	}

	return RETCODE_SUCCESS;
}